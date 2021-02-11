#include "Entity.h"

Entity::Entity()
{
	
}

Entity::Entity(string entityName)
{
	name = new string();
	materialMap = new map<string, Material*>;
	children = new vector<Entity*>;
	colliders = new vector<Collider*>;
	
	*name = entityName;
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotationInDegrees = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	repeatTex = XMFLOAT2(1.0f, 1.0f);
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));
	isEmptyObj = true;
}

Entity::Entity(string entityName, Mesh* entityMesh, Material* mat)
{
	name = new string();
	materialMap = new map<string, Material*>;
	children = new vector<Entity*>;
	colliders = new vector<Collider*>;

	mesh = entityMesh;
	*name = entityName;
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotationInDegrees = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	repeatTex = XMFLOAT2(1.0f, 1.0f);
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));
	if (mat != nullptr)
		materialMap->insert({ mat->GetName(), mat });
}


Entity::~Entity()
{
	if (colliders != nullptr) {
		for (size_t i = 0; i < colliders->size(); i++)
		{
			delete (*colliders)[i];
		}
		colliders->empty();
	}

	if(dynamicsWorld != nullptr)
		dynamicsWorld->removeCollisionObject(rBody);
	if (rBody != nullptr) {
		delete rBody->getMotionState();
		delete rBody;
	}
	
	if(collShape != nullptr)
		delete collShape;

	if (materialMap != nullptr) {
		materialMap->empty();
		delete materialMap;
	}
		
	if (children != nullptr) {
		children->empty();
		delete children;
	}
	if (colliders != nullptr)
		delete colliders;

	delete name;
}

void Entity::operator=(const Entity& e)
{
	name = new string();
	materialMap = new map<string, Material*>();
	children = new vector<Entity*>();
	colliders = new vector<Collider*>();

	*name = *e.name;
	*children = vector<Entity*>(*e.children);
	*colliders = vector<Collider*>(*e.colliders);
	*materialMap = map<string, Material*>(*e.materialMap);
	worldMatrix = e.worldMatrix;
	mesh = e.mesh;
	quaternion = e.quaternion;
	position = e.position;
	scale = e.scale;
	rotation = e.rotation;
	rotationInDegrees = e.rotationInDegrees;
	repeatTex = e.repeatTex;
	parent = e.parent;
	shadowsEnabled = e.shadowsEnabled;
	shadowData = e.shadowData;
	isStatic = e.isStatic;
	collShape = e.collShape;
	rBody = e.rBody;
	dynamicsWorld = e.dynamicsWorld;
	destroyed = e.destroyed;
	isCollisionStatic = e.isCollisionStatic;
	collisionsEnabled = e.collisionsEnabled;
	colliderDebugLinesEnabled = e.colliderDebugLinesEnabled;
	isEmptyObj = e.isEmptyObj;
	meshMaterialIndex = e.meshMaterialIndex;
}

void Entity::InitRigidBody(btDiscreteDynamicsWorld* dw)
{
	dynamicsWorld = dw;

	// Physics set-up
	this->collShape = new btBoxShape(btVector3(btScalar(scale.x / 2.0f), btScalar(scale.y / 2.0f), btScalar(scale.z / 2.0f)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
	btQuaternion qx = btQuaternion(btVector3(1.0f, 0.0f, 0.0f), rotation.x);
	btQuaternion qy = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), rotation.y);
	btQuaternion qz = btQuaternion(btVector3(0.0f, 0.0f, 1.0f), rotation.z);
	btQuaternion res = qz * qy * qx;
	groundTransform.setRotation(res);
	//groundTransform.setRotation(btQuaternion(rotation.y, rotation.x, rotation.z));

	//btScalar mass(isStatic);
	btScalar mass(0.0f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		collShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, collShape, localInertia);
	this->rBody = new btRigidBody(rbInfo);

	//rBody->setLinearFactor(btVector3(1, 1, 0));
	//rBody->setAngularFactor(btVector3(0, 1, 1));

	rBody->setAnisotropicFriction(btVector3(2.0f, 0.0f, 0.0f));

	//rBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);

	// Have the rigid body register a pointer to the entity it belongs to so we can access it
	rBody->setUserPointer((void*)(this));

	dynamicsWorld->addRigidBody(rBody);
}

DirectX::XMFLOAT4X4 Entity::GetWorldMatrix()
{
	return worldMatrix;
}

DirectX::XMFLOAT3 Entity::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Entity::GetScale()
{
	return scale;
}

DirectX::XMFLOAT3 Entity::GetEulerAngles()
{
	return rotation;
}

DirectX::XMFLOAT3 Entity::GetEulerAnglesDegrees()
{
	return rotationInDegrees;
}

DirectX::XMFLOAT4 Entity::GetRotationQuaternion()
{
	return quaternion;
}

void Entity::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Entity::SetPosition(XMFLOAT3 p)
{
	position = p;
}

void Entity::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Entity::SetScale(XMFLOAT3 s)
{
	scale = s;
}

///<summary>
///Rotation in Radians
///</summary>
void Entity::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;

	rotationInDegrees.x = DirectX::XMConvertToDegrees(x);
	rotationInDegrees.y = DirectX::XMConvertToDegrees(y);
	rotationInDegrees.z = DirectX::XMConvertToDegrees(z);

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(x, y, z);
	XMStoreFloat4(&quaternion, quat);
}

void Entity::SetRotation(XMFLOAT4 quat)
{
	quaternion = quat;
	CalcEulerAngles();
}

///<summary>
///Rotation in Radians
///</summary>
void Entity::SetRotation(XMFLOAT3 rotRadians)
{
	rotation = rotRadians;

	rotationInDegrees.x = DirectX::XMConvertToDegrees(rotation.x);
	rotationInDegrees.y = DirectX::XMConvertToDegrees(rotation.y);
	rotationInDegrees.z = DirectX::XMConvertToDegrees(rotation.z);

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat4(&quaternion, quat);
}

void Entity::RotateAroundAxis(XMFLOAT3 axis, float scalar)
{
	XMVECTOR a = XMLoadFloat3(&axis);
	XMVECTOR quat = XMQuaternionRotationAxis(a, scalar);
	XMVECTOR existingQuat = XMLoadFloat4(&quaternion);
	XMVECTOR result = XMQuaternionMultiply(existingQuat, quat);
	XMStoreFloat4(&quaternion, result);
	CalcEulerAngles();
}

void Entity::CalcEulerAngles()
{
	XMVECTOR q = XMLoadFloat4(&quaternion);

	XMFLOAT3 x = X_AXIS;
	XMFLOAT3 y = X_AXIS;
	XMFLOAT3 z = X_AXIS;

	XMVECTOR xs = XMLoadFloat3(&x);
	XMVECTOR ys = XMLoadFloat3(&y);
	XMVECTOR zs = XMLoadFloat3(&z);

	XMQuaternionToAxisAngle(&xs, &rotation.x, q);
	XMQuaternionToAxisAngle(&ys, &rotation.y, q);
	XMQuaternionToAxisAngle(&zs, &rotation.z, q);
}

void Entity::SetRepeatTexture(float x, float y)
{
	repeatTex = XMFLOAT2(x, y);
}

void Entity::SetShadowData(ShadowData shadowData)
{
	this->shadowData = shadowData;
}

void Entity::SetMeshAndMaterial(Mesh* mesh, Material* mat)
{
	this->mesh = mesh;
	if (mat != nullptr)
		materialMap->insert({ mat->GetName(), mat });
}

void Entity::ToggleShadows(bool toggle)
{
	shadowsEnabled = toggle;
}

void Entity::Move(XMFLOAT3 f)
{
	position.x += f.x;
	position.y += f.y;
	position.z += f.z;
	CalcWorldMatrix();
}

void Entity::Move(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	CalcWorldMatrix();
}

ID3D11Buffer * Entity::GetMeshVertexBuffer(int childIndex)
{
	if(childIndex == -1)
		return mesh->GetVertexBuffer();
	else
		return mesh->GetChildren()[childIndex]->GetVertexBuffer();
}

ID3D11Buffer * Entity::GetMeshIndexBuffer(int childIndex)
{
	if (childIndex == -1)
		return mesh->GetIndexBuffer();
	else
		return mesh->GetChildren()[childIndex]->GetIndexBuffer();
}

int Entity::GetMeshIndexCount(int childIndex)
{
	if (childIndex == -1)
		return mesh->GetIndexCount();
	else
		return mesh->GetChildren()[childIndex]->GetIndexCount();
}

string Entity::GetMeshMaterialName(int childIndex)
{
	if (childIndex == -1)
		return mesh->GetMaterialName(meshMaterialIndex);
	else
		return mesh->GetChildren()[childIndex]->GetMaterialName();
}

//Calculate after every transformation
void Entity::CalcWorldMatrix()
{
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	//DirectX::XMMATRIX rot   = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR quat = XMLoadFloat4(&quaternion);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(quat);
	DirectX::XMMATRIX scl   = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX world = scl * rot * trans;
	if (parent != nullptr) {
		DirectX::XMMATRIX parentWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&parent->worldMatrix));
		world = DirectX::XMMatrixMultiply(world, parentWorld);
	}
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(world));
	for (size_t i = 0; i < children->size(); i++)
	{
		(*children)[i]->CalcWorldMatrix();
	}
	for (size_t i = 0; i < colliders->size(); i++)
	{
		(*colliders)[i]->SetWorldMatrix(worldMatrix);
	} 
}

void Entity::PrepareMaterialForDraw(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	SimpleVertexShader* vs = (*materialMap)[n]->GetVertexShader();
	SimplePixelShader* ps = (*materialMap)[n]->GetPixelShader();

	// Send data to shader variables
		//  - Do this ONCE PER OBJECT you're drawing
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.
	vs->SetMatrix4x4("world", GetWorldMatrix());
	vs->SetMatrix4x4("view", view);
	vs->SetMatrix4x4("projection", proj);

	ps->SetData(
		"uvMult",
		&repeatTex,
		sizeof(repeatTex)
	);

	if (shadowsEnabled) {
		vs->SetMatrix4x4("shadowView", shadowData.shadowViewMatrix);
		vs->SetMatrix4x4("shadowProj", shadowData.shadowProjectionMatrix);
		ps->SetShaderResourceView("ShadowMap", shadowData.shadowSRV);
		ps->SetSamplerState("ShadowSampler", shadowData.shadowSampler);
	}

	(*materialMap)[n]->Prepare();
}

Material * Entity::GetMaterial(string n)
{
	return (*materialMap)[n];
}

bool Entity::MeshHasChildren()
{
	return mesh->HasChildren();
}

int Entity::GetMeshChildCount()
{
	return mesh->GetChildCount();
}

Mesh* Entity::GetMesh()
{
	return mesh;
}

vector<string> Entity::GetMaterialNameList()
{
	return mesh->GetMaterialNameList();
}

void Entity::AddMaterial(Material * mat, bool addToMesh)
{
	//if (materialMap->size() == 0) materialMap = map<string, Material*>();
	string nm = mat->GetName();
	if (materialMap->count(nm)) return;
	materialMap->insert({ nm, mat });
	if(addToMesh)
		meshMaterialIndex = mesh->AddMaterialName(nm);
}

string Entity::GetName()
{
	return *name;
}

void Entity::AddChildEntity(Entity* child)
{
	children->push_back(child);
	child->parent = this;
}

void Entity::AddAutoBoxCollider()
{
	if (mesh->HasChildren()) {
		Mesh** children = mesh->GetChildren();
		for (size_t i = 0; i < mesh->GetChildCount(); i++)
		{
			colliders->push_back(new Collider(children[i]->GetVertices()));
		}
	}
	else {
		colliders->push_back(new Collider(mesh->GetVertices()));
	}
}

bool Entity::CheckSATCollision(Entity* other)
{
	bool otherHasChildren = other->MeshHasChildren();
	unsigned int result;
	for (size_t i = 0; i < (mesh->HasChildren() ? mesh->GetChildCount() : 1); i++)
	{
		for (size_t j = 0; j < (other->MeshHasChildren() ? other->GetMeshChildCount() : 1); j++)
		{
			if((*colliders)[i]->CheckSATCollision((*other->colliders)[j]) == -1) return true;
		}
	}
	return false;
}

bool Entity::CheckSATCollisionAndCorrect(Entity* other)
{
	if (isCollisionStatic) return false;
	bool isColliding;
	XMFLOAT3 result;
	for (size_t i = 0; i < (mesh->HasChildren() ? mesh->GetChildCount() : 1); i++)
	{
		for (size_t j = 0; j < (other->MeshHasChildren() ? other->GetMeshChildCount() : 1); j++)
		{
			isColliding = (*colliders)[i]->CheckSATCollisionForCorrection((*other->colliders)[j], result);
			if (isColliding) break;
		}
	}
	
	if (isColliding) {
		XMVECTOR modifiedVec = XMLoadFloat3(&result);
		XMVECTOR dist;
		XMVECTOR thisPos = XMLoadFloat3(&position);
		XMVECTOR otherPos = XMLoadFloat3(&other->position);
		dist = XMVectorSubtract(thisPos, otherPos);
		XMVECTOR dotV = XMVector3Dot(modifiedVec, dist);
		float dot;
		XMStoreFloat(&dot, dotV);
		if (dot < 0.0f) {
			modifiedVec = XMVectorScale(modifiedVec, -1.0f);
		}
		if (!other->isCollisionStatic) {
			XMFLOAT3 otherResult;
			modifiedVec = XMVectorScale(modifiedVec, 0.5f);
			XMVECTOR otherResultVec = XMVectorScale(modifiedVec, -1.0f);
			XMStoreFloat3(&otherResult, otherResultVec);
			other->Move(otherResult);
		}
		XMFLOAT3 modifiedResult;
		XMStoreFloat3(&modifiedResult, modifiedVec);
		Move(modifiedResult);
		return true;
	}
	else return false;
}

vector<Collider*> Entity::GetColliders()
{
	return (*colliders);
}

btRigidBody* Entity::GetRBody()
{
	return rBody;
}

Collider* Entity::GetCollider(int index)
{
	return (*colliders)[index];
}

void Entity::Destroy()
{
	this->destroyed = true;
	for (size_t i = 0; i < children->size(); i++)
	{
		(*children)[i]->Destroy();
	}
}

void Entity::FreeMemory()
{
	if (colliders != nullptr) {
		for (size_t i = 0; i < colliders->size(); i++)
		{
			delete (*colliders)[i];
		}
		colliders->empty();
	}

	if (dynamicsWorld != nullptr)
		dynamicsWorld->removeCollisionObject(rBody);
	if (rBody != nullptr) {
		delete rBody->getMotionState();
		delete rBody;
	}

	if (collShape != nullptr)
		delete collShape;

	if (materialMap != nullptr) {
		materialMap->empty();
		delete materialMap;
	}

	if (children != nullptr) {
		children->empty();
		delete children;
	}
	if (colliders != nullptr)
		delete colliders;

	delete name;
}


