#include "pch.h"
#include "Barrel.h"

float Vec3Dot(Vec3 a, Vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3Cross(Vec3 a, Vec3 b) {
	return { (a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x) };
}

Vec3 RotateVec3(Vec3 vector, Quaternion quat) {
	//where u is the vector component of quaternion
	//where s is the scalar component of quaternion
	// = 2(u⋅v)u + (s^2−u⋅u)v + 2s(u×v)

	Vec3 u = { quat.x, quat.y, quat.z };
	float s = quat.w;

	return u * 2.0f * Vec3Dot(u, vector) + vector * (s * s - Vec3Dot(u, u)) + Vec3Cross(u, vector) * 2 * s;
}

Vec3 NormalizeVec3(Vec3 vector) {
	float mag = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	return vector * (1 / mag);
}

void TestScript::Init()
{
	frustum = { 0.1f, 1000.0f, ZERO_VECTOR3, Utility::DegToRad(90), 1600.0f / 900.0f };
	frustum.CalcFrustumPlanes();

	mat = MATRIX_IDENTITY;

	XMFLOAT3 y = Y_AXIS;
	XMFLOAT3 x = Z_AXIS;
	XMVECTOR quat = XMQuaternionMultiply(XMQuaternionRotationAxis(XMLoadFloat3(&y), XM_PIDIV2), XMQuaternionRotationAxis(XMLoadFloat3(&x), XM_PIDIV4));

	Quaternion q = { quat.m128_f32[0], quat.m128_f32[1], quat.m128_f32[2], quat.m128_f32[3] };

	/*
	for (size_t i = 6; i < 12; i++)
	{
		//if (i != 3) continue;
		// Get the unprojected vector of the mouse click position in world space
		XMFLOAT3 start = ZERO_VECTOR3;
		XMFLOAT3 end = XMFLOAT3(frustum.planes[i].a, frustum.planes[i].b, frustum.planes[i].c);
		//Vec3 newend = RotateVec3({ frustum.planes[i].a, frustum.planes[i].b, frustum.planes[i].c }, q);
		//XMFLOAT3 end = XMFLOAT3(newend.x, newend.y, newend.z);

		// Create debug line
		DebugLines* dl = new DebugLines("TestRay", 0, false);
		XMFLOAT3 c = XMFLOAT3(0.0f, 1.0f, 0.0f);
		dl->color = c;
		dl->worldMatrixPtr = &mat;

		// Draw the debug line to show the raycast
		XMFLOAT3* rayPoints = new XMFLOAT3[8];
		rayPoints[0] = start;
		rayPoints[1] = start;
		rayPoints[2] = start;
		rayPoints[3] = start;
		rayPoints[4] = end;
		rayPoints[5] = end;
		rayPoints[6] = end;
		rayPoints[7] = end;
		dl->GenerateCuboidVertexBuffer(rayPoints, 8);
		delete[] rayPoints;
	}
	*/

	obstacles = new AABB[1];
	obstacles[0].min = { -0.5f, -0.5f, 2.5f };
	obstacles[0].max = { 0.5f, 0.5f, 3.5f };
	obstacleCnt = 1;

	cam = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f,0.0f}, frustum };

	cubePoint = { 0.0f,0.0f,3.0f };
	testPoint = { 1.0f,1.0f,5.00f };

	XMFLOAT3 start = ZERO_VECTOR3;
	XMFLOAT3 end = testPoint;
	//Vec3 newend = RotateVec3({ frustum.planes[i].a, frustum.planes[i].b, frustum.planes[i].c }, q);
	//XMFLOAT3 end = XMFLOAT3(newend.x, newend.y, newend.z);

	// Create debug line
	DebugLines* dl = new DebugLines("TestRay", 0, false);
	XMFLOAT3 c = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dl->color = c;
	dl->worldMatrixPtr = &mat;

	// Draw the debug line to show the raycast
	XMFLOAT3* rayPoints = new XMFLOAT3[8];
	rayPoints[0] = start;
	rayPoints[1] = start;
	rayPoints[2] = start;
	rayPoints[3] = start;
	rayPoints[4] = end;
	rayPoints[5] = end;
	rayPoints[6] = end;
	rayPoints[7] = end;
	dl->GenerateCuboidVertexBuffer(rayPoints, 8);

	dl = new DebugLines("TestRay", 0, false);
	c = XMFLOAT3(1.0f, 0.0f, 0.0f);
	dl->color = c;
	dl->worldMatrixPtr = &mat;
	end = { 1.0f,0,0 };
	rayPoints[0] = start;
	rayPoints[1] = start;
	rayPoints[2] = start;
	rayPoints[3] = start;
	rayPoints[4] = end;
	rayPoints[5] = end;
	rayPoints[6] = end;
	rayPoints[7] = end;
	dl->GenerateCuboidVertexBuffer(rayPoints, 8);

	dl = new DebugLines("TestRay", 0, false);
	c = XMFLOAT3(0.0f, 1.0f, 0.0f);
	dl->color = c;
	dl->worldMatrixPtr = &mat;
	end = { 0.0f,1,0 };
	rayPoints[0] = start;
	rayPoints[1] = start;
	rayPoints[2] = start;
	rayPoints[3] = start;
	rayPoints[4] = end;
	rayPoints[5] = end;
	rayPoints[6] = end;
	rayPoints[7] = end;
	dl->GenerateCuboidVertexBuffer(rayPoints, 8);

	dl = new DebugLines("TestRay", 0, false);
	c = XMFLOAT3(0.0f, 0.0f, 1.0f);
	dl->color = c;
	dl->worldMatrixPtr = &mat;
	end = { 0.0f,0,1 };
	rayPoints[0] = start;
	rayPoints[1] = start;
	rayPoints[2] = start;
	rayPoints[3] = start;
	rayPoints[4] = end;
	rayPoints[5] = end;
	rayPoints[6] = end;
	rayPoints[7] = end;
	dl->GenerateCuboidVertexBuffer(rayPoints, 8);

	delete[] rayPoints;

	(*sceneEntitiesMap)["testcube"]->GetTransformPtr()->SetPosition(cubePoint);

	FPSCtrlUICb.spriteBatch = new SpriteBatch(Config::Context);
	FPSCtrlUICb.font = EESceneLoader->FontMap["Arial"];
	EERenderer->SetRenderUICallback(true, &FPSCtrlUICb, 1);
}

void TestScript::Update()
{
	/*
	//Entity* fps = EESceneLoader->sceneEntitiesMap["FPSController"];
	Camera* cam = EERenderer->GetCamera("main");
	XMFLOAT3 camPos = cam->position;
	XMFLOAT3 camDir = cam->direction;
	entity->SetPosition(XMFLOAT3(camPos.x + camDir.x, camPos.y + camDir.y, camPos.z + camDir.z));
	entity->CalcWorldMatrix();
	*/
	//collision barrels

	XMFLOAT3 campos = Renderer::GetInstance()->GetCamera("main")->GetTransform().GetPosition();
	XMFLOAT4 camquat = Renderer::GetInstance()->GetCamera("main")->GetTransform().GetRotationQuaternion();

	cam.position = { campos.x, campos.y, campos.z };
	cam.rotation = { camquat.x, camquat.y, camquat.z, camquat.w };

	if (GetAsyncKeyState(VK_LEFT))
	{
		(*sceneEntitiesMap)["testcube"]->GetTransform().Move(-0.05f, 0, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		(*sceneEntitiesMap)["testcube"]->GetTransform().Move(0.05f, 0, 0);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		(*sceneEntitiesMap)["testcube"]->GetTransform().Move(0, 0, 0.05f);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		(*sceneEntitiesMap)["testcube"]->GetTransform().Move(0, 0, -0.05f);
	}

	if (IsVisible(cam, { testPoint.x, testPoint.y, testPoint.z }, obstacles, obstacleCnt)) {
		FPSCtrlUICb.playerPos = "visible";
	}
	else {
		FPSCtrlUICb.playerPos = "culled";
	}

	/*
	if (entity->CheckSATCollisionAndCorrect((*eMap)["Rock (1)"]))
	{
		cout << test << endl;
	}
	*/

	/*
	//ruin
	if (GetAsyncKeyState('B') & 0x8000) {
		entity->RotateAroundAxis(Z_AXIS, -0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('N') & 0x8000) {
		entity->RotateAroundAxis(Z_AXIS, 0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('U') & 0x8000) {
		entity->RotateAroundAxis(X_AXIS, -0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('I') & 0x8000) {
		entity->RotateAroundAxis(X_AXIS, 0.05f);
		entity->CalcWorldMatrix();
	}

	/*
	//transform hierarchy spheres
	if (GetAsyncKeyState('F') & 0x8000) {
		(*eMap)["cube1"]->RotateAroundAxis(Y_AXIS, 0.05f);
		(*eMap)["cube1"]->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('G') & 0x8000) {
		(*eMap)["cube1"]->RotateAroundAxis(Y_AXIS, -0.05f);
		(*eMap)["cube1"]->CalcWorldMatrix();
	}

	//delete barrel
	if (GetAsyncKeyState('L') & 0x8000) {
		entity->Destroy();
	}
	*/
}

void TestScript::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;
	//cout << otherE->GetName() << endl;
}

bool TestScript::IsVisible(const SCamera& camera, const Vec3& targetPosition, const AABB* obstacles, size_t numObstacles)
{
	Vec3 plane;
	float dist;
	Vec3 ray = targetPosition;
	ray = ray - camera.position;
	for (size_t i = 0; i < 6; i++)
	{
		plane = RotateVec3({ frustum.planes[i].a, frustum.planes[i].b, frustum.planes[i].c }, camera.rotation);
		dist = Vec3Dot(ray, plane) + frustum.planes[i].d;
		if (dist < 0) {
			return false;
		}
	}

	float tMinX = 0.0f, tMaxX = 0.0f, tMinY = 0.0f, tMaxY = 0.0f, tMinZ = 0.0f, tMaxZ = 0.0f;
	float tMin = 0.0f, tMax = 0.0f;
	AABB cObstacle;
	Vec3 rayDir = NormalizeVec3(ray);
	float rayMag = sqrt(ray.x * ray.x + ray.y * ray.y + ray.z * ray.z);

	//get raycast t values for each obstacle so we can check if ray is at all obstructed
	for (size_t i = 0; i < numObstacles; i++)
	{
		cObstacle = obstacles[i];
		tMinX = (cObstacle.min.x - camera.position.x) / rayDir.x;
		tMaxX = (cObstacle.max.x - camera.position.x) / rayDir.x;
		tMinY = (cObstacle.min.y - camera.position.y) / rayDir.y;
		tMaxY = (cObstacle.max.y - camera.position.y) / rayDir.y;
		tMinZ = (cObstacle.min.z - camera.position.z) / rayDir.z;
		tMaxZ = (cObstacle.max.z - camera.position.z) / rayDir.z;

		//swap variables if necessary depending on camera orientation
		if (tMinX > tMaxX) {
			float tMinXTemp = tMinX;
			tMinX = tMaxX;
			tMaxX = tMinXTemp;
		}
		if (tMinY > tMaxY) {
			float tMinYTemp = tMinY;
			tMinY = tMaxY;
			tMaxY = tMinYTemp;
		}
		if (tMinZ > tMaxZ) {
			float tMinZTemp = tMinZ;
			tMinZ = tMaxZ;
			tMaxZ = tMinZTemp;
		}

		if ((tMinX > tMaxY) || (tMinY > tMaxX)) continue;
		if ((tMinX > tMaxZ) || (tMinZ > tMaxX)) continue;


		//if ray intersects with an obstacle on any axis then the point is not visible, we dont care about the actual t values
		if ((tMinX < rayMag) &&
			(tMinY < rayMag) &&
			(tMinZ < rayMag))
			return false;
	}

	return true;
}