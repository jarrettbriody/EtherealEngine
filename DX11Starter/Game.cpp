#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		// The application's handle
		"DirectX Game",	   	// Text for the window's title bar
		1280,			// Width of the window's client area
		720,			// Height of the window's client area
		true)			// Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	for (size_t i = 0; i < 6; i++)
	{
		delete entities[i];
	}
	delete mesh1;
	delete mesh2;
	delete mesh3;

	delete sphere;
	delete cone;
	delete cube;
	delete cylinder;
	delete helix;
	delete torus;

	delete camera;

	marbleSRV->Release();
	hedgeSRV->Release();
	sampler->Release();

	delete material;
	delete material2;

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	camera = new Camera();

	CreateMatrices();
	CreateBasicGeometry();

	DirectX::CreateWICTextureFromFile(device, context, L"../../Assets/Textures/marble.png", 0, &marbleSRV);

	DirectX::CreateWICTextureFromFile(device, context, L"../../Assets/Textures/hedge.jpg", 0, &hedgeSRV);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);

	material = new Material(vertexShader, pixelShader, marbleSRV, sampler);
	material2 = new Material(vertexShader, pixelShader, hedgeSRV, sampler);

	entities[0] = new Entity(cone, material);
	entities[1] = new Entity(cube, material);

	entities[2] = new Entity(cylinder, material);
	entities[3] = new Entity(helix, material2);

	entities[4] = new Entity(torus, material2);
	entities[5] = new Entity(mesh3, material);

	entities[6] = new Entity(sphere, material2);

	prevMousePos.x = 0;
	prevMousePos.y = 0;

	dLight.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dLight.DiffuseColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	dLight.Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);

	dLight2.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dLight2.DiffuseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	dLight2.Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	dLight3.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dLight3.DiffuseColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	dLight3.Direction = XMFLOAT3(0.0f, 1.0f, 0.0f);

	pixelShader->SetData(
		"light",
		&dLight,
		sizeof(DirectionalLight)
	);
	pixelShader->SetData(
		"light2",
		&dLight2,
		sizeof(DirectionalLight)
	);
	pixelShader->SetData(
		"light3",
		&dLight3,
		sizeof(DirectionalLight)
	);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	camera->UpdateProjectionMatrix(width, height);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	sphere = new Mesh("../../Assets/Models/sphere.obj", device);
	cone = new Mesh("../../Assets/Models/cone.obj", device);
	cube = new Mesh("../../Assets/Models/cube.obj", device);
	cylinder = new Mesh("../../Assets/Models/cylinder.obj", device);
	helix = new Mesh("../../Assets/Models/helix.obj", device);
	torus = new Mesh("../../Assets/Models/torus.obj", device);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	Vertex vertices2[] =
	{
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
	};

	unsigned int indices2[] = { 0, 1, 3, 3, 1, 2 };

	Vertex vertices3[] =
	{
		{ XMFLOAT3(+0.0f, +2.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(+2.5f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(+0.0f, +0.0f) },
	};

	unsigned int indices3[] = { 0, 1, 2 };

	mesh1 = new Mesh(vertices, 3, indices, 3, device);

	mesh2 = new Mesh(vertices2, 4, indices2, 6, device);

	mesh3 = new Mesh(vertices3, 3, indices3, 3, device);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->UpdateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera->Update();

	entities[0]->Move(sin(totalTime) * deltaTime, 0.0f, 0.0f);
	entities[1]->Move(0.1f * deltaTime, 0.0f, 0.0f);
	entities[2]->Move(-sin(totalTime) * deltaTime, 0.0f, 0.0f);
	entities[3]->Move(0.0f, 0.1f * deltaTime, 0.0f);
	entities[4]->Move(0.1f * deltaTime, -0.1f * deltaTime, 0.0f);
	entities[5]->Move(0.0f, 0.0f, 0.1f * deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (size_t i = 0; i < 7; i++)
	{
		ID3D11Buffer* vbo = entities[i]->GetMeshVertexBuffer();
		context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
		context->IASetIndexBuffer(entities[i]->GetMeshIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		entities[i]->PrepareMaterial(camera->GetViewMatrix(), camera->GetProjMatrix());

		pixelShader->SetSamplerState("BasicSampler", entities[i]->GetMaterial()->GetSamplerState());
		pixelShader->SetShaderResourceView("DiffuseTexture", entities[i]->GetMaterial()->GetShaderResourceView());

		// Finally do the actual drawing
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		context->DrawIndexed(
			entities[i]->GetMeshIndexCount(),   // The number of indices to use (we could draw a subset if we wanted)
			0,									// Offset to the first index we want to use
			0);									// Offset to add to each index when looking up vertices
	}



	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	camera->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion