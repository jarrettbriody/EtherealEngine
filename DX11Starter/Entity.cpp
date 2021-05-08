#include "pch.h"
#include "Entity.h"

Entity::Entity()
{
	ZeroMemory(tags, MAX_ENTITY_TAG_COUNT * sizeof(EEString<EESTRING_SIZE>));
	ZeroMemory(layers, MAX_ENTITY_LAYER_COUNT * sizeof(EEString<EESTRING_SIZE>));
}

Entity::Entity(string entityName)
{
	materialMap = new map<string, Material*>;
	children = new vector<Entity*>;
	colliders = new vector<Collider*>;
	ZeroMemory(tags, MAX_ENTITY_TAG_COUNT * sizeof(EEString<EESTRING_SIZE>));
	ZeroMemory(layers, MAX_ENTITY_LAYER_COUNT * sizeof(EEString<EESTRING_SIZE>));
	
	name = entityName;
	isEmptyObj = true;
}

Entity::Entity(string entityName, Mesh* entityMesh, Material* mat)
{
	materialMap = new map<string, Material*>;
	children = new vector<Entity*>;
	colliders = new vector<Collider*>;
	ZeroMemory(tags, MAX_ENTITY_TAG_COUNT * sizeof(EEString<EESTRING_SIZE>));
	ZeroMemory(layers, MAX_ENTITY_LAYER_COUNT * sizeof(EEString<EESTRING_SIZE>));

	mesh = entityMesh;
	name = entityName;
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
		colliders->clear();
	}

	if(Config::DynamicsWorld != nullptr && rBody != nullptr)
	{
		Config::DynamicsWorld->removeCollisionObject(rBody);
		delete rBody->getMotionState();
		delete rBody;
	}
	
	if (collShape != nullptr) {
		for (size_t i = 0; i < colliderCnt; i++)
		{
			delete collShape[i];
		}
		delete[] collShape;
	}

	if (compoundShape != nullptr)
		delete compoundShape;

	if (materialMap != nullptr) {
		materialMap->clear();
		delete materialMap;
	}
		
	if (children != nullptr) {
		children->clear();
		delete children;
	}
	if (colliders != nullptr)
		delete colliders;

}

void Entity::operator=(const Entity& e)
{
	materialMap = new map<string, Material*>();
	children = new vector<Entity*>();
	colliders = new vector<Collider*>();
	ZeroMemory(tags, MAX_ENTITY_TAG_COUNT * sizeof(EEString<EESTRING_SIZE>));
	ZeroMemory(layers, MAX_ENTITY_LAYER_COUNT * sizeof(EEString<EESTRING_SIZE>));

	name = e.name;
	eTransform = e.eTransform;
	tagCount = e.tagCount;
	layerCount = e.layerCount;
	layerMask = e.layerMask;
	memcpy(tags, e.tags, sizeof(EEString<EESTRING_SIZE>) * tagCount);
	memcpy(layers, e.layers, sizeof(EEString<EESTRING_SIZE>) * layerCount);
	*children = vector<Entity*>(*e.children);
	*colliders = vector<Collider*>(*e.colliders);
	*materialMap = map<string, Material*>(*e.materialMap);
	mesh = e.mesh;
	repeatTex = e.repeatTex;
	uvOffset = e.uvOffset;
	parent = e.parent;
	shadowsEnabled = e.shadowsEnabled;
	shadowData = e.shadowData;
	mass = e.mass;
	collShape = e.collShape;
	compoundShape = e.compoundShape;
	rBody = e.rBody;
	destroyed = e.destroyed;
	isCollisionStatic = e.isCollisionStatic;
	collisionsEnabled = e.collisionsEnabled;
	colliderDebugLinesEnabled = e.colliderDebugLinesEnabled;
	isEmptyObj = e.isEmptyObj;
	meshMaterialIndex = e.meshMaterialIndex;
	colliderCnt = e.colliderCnt;
	renderObject = e.renderObject;
	pWrap = e.pWrap;
	hbaoPlusEnabled = e.hbaoPlusEnabled;

}

Transform& Entity::GetTransform()
{
	return eTransform;
}

void Entity::InitRigidBody(BulletColliderShape shape, float entityMass, bool zeroObjects)
{
	assert(colliderCnt > 0);

	mass = entityMass;

	collShape = new btCollisionShape * [colliderCnt];
	
	if (colliderCnt > 1) {
		compoundShape = new btCompoundShape();
	}

	// Physics set-up
	for (size_t i = 0; i < colliderCnt; i++)
	{
		XMFLOAT3 span = (*colliders)[i]->GetHalfWidth();
		XMVECTOR spanVec = XMLoadFloat3(&span);
		spanVec = XMVectorScale(spanVec, 1.0f);
		XMStoreFloat3(&span, spanVec);
		XMFLOAT3 centerLocal = (*colliders)[i]->GetCenterLocal();
		XMVECTOR centerLocalCalc = XMLoadFloat3(&centerLocal);
		centerLocalCalc = XMVector3Length(centerLocalCalc);
		XMFLOAT3 res;
		XMStoreFloat3(&res, centerLocalCalc);
		float mag = res.x;

		switch (shape)
		{
		case BulletColliderShape::BOX:
			collShape[i] = new btBoxShape(btVector3(btScalar(span.x), btScalar(span.y), btScalar(span.z)));
			break;
		case BulletColliderShape::CAPSULE:
			collShape[i] = new btCapsuleShape(btScalar(span.x), btScalar(span.y * 2));
			break;
		default:
			break;
		}

		collShape[i]->setUserPointer((*colliders)[i]);

		if (mag > 0.001f || this->compoundShape != nullptr) {
			if(this->compoundShape == nullptr) this->compoundShape = new btCompoundShape();
			btTransform localTransform;
			localTransform.setIdentity();
			localTransform.setOrigin(btVector3(centerLocal.x, centerLocal.y, centerLocal.z));
			this->compoundShape->addChildShape(localTransform, this->collShape[i]);
		}
		else {
			XMFLOAT3 scl = eTransform.GetScale();
			collShape[i]->setLocalScaling(btVector3(scl.x, scl.y, scl.z));
		}
	}

	btTransform transform;
	transform.setIdentity();
	//XMFLOAT3 centerLocal = GetCollider()->GetCenterLocal();
	//XMFLOAT3 scale = GetScale();
	//centerLocal = XMFLOAT3(centerLocal.x * scale.x, centerLocal.y * scale.y, centerLocal.z * scale.z);
	//transform.setOrigin(btVector3(position.x + centerLocal.x, position.y + centerLocal.y, position.z + centerLocal.z));

	XMFLOAT3 position = eTransform.GetPosition();
	XMFLOAT3 rotation = eTransform.GetEulerAnglesRadians();
	XMFLOAT3 scale = eTransform.GetScale();

	transform.setOrigin(btVector3(position.x, position.y, position.z));
	btQuaternion qx = btQuaternion(btVector3(1.0f, 0.0f, 0.0f), rotation.x);
	btQuaternion qy = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), rotation.y);
	btQuaternion qz = btQuaternion(btVector3(0.0f, 0.0f, 1.0f), rotation.z);
	btQuaternion res = qz * qy * qx;
	transform.setRotation(res);
	//groundTransform.setRotation(btQuaternion(rotation.y, rotation.x, rotation.z));

	//btScalar mass(isStatic);
	//btScalar mass(0.0f);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.0f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic) {
		for (size_t i = 0; i < colliderCnt; i++)
		{
			collShape[i]->calculateLocalInertia(mass, localInertia);
		}
	}

	btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);

	if (colliderCnt == 1 && compoundShape != nullptr && zeroObjects) {
		XMFLOAT3 centerLocal = (*colliders)[0]->GetCenterLocal();
		btTransform localTransform;
		localTransform.setIdentity();
		localTransform.setOrigin(btVector3(centerLocal.x * scale.x, centerLocal.y * scale.y, centerLocal.z * scale.z));
		myMotionState->m_centerOfMassOffset = localTransform;
	}

	if (compoundShape == nullptr) {
		for (size_t i = 0; i < colliderCnt; i++)
		{
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, collShape[i], localInertia);
			this->rBody = new btRigidBody(rbInfo);
		}
		
	}
	else {
		this->compoundShape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		this->compoundShape->setUserPointer(colliders);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
		this->rBody = new btRigidBody(rbInfo);
	}


	rBody->setFriction(0.5f);

	/*rBody->setActivationState(DISABLE_DEACTIVATION);
	rBody->setMassProps(mass, localInertia);*/

	//rBody->setLinearFactor(btVector3(1, 1, 0));
	//rBody->setAngularFactor(btVector3(0, 1, 1));

	//rBody->setAnisotropicFriction(btVector3(2.0f, 0.0f, 0.0f));

	if(mass == 0.0f)
		rBody->setCollisionFlags(rBody->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// Have the rigid body register a pointer to the entity it belongs to so we can access it
	// TODO: Change this to a struct with everything included (name, entity pointer, tag)
	pWrap = { PHYSICS_WRAPPER_TYPE::ENTITY, (void*)(this) };
	rBody->setUserPointer(&pWrap);

	Config::DynamicsWorld->addRigidBody(rBody);
}

// TODO: Make sure this method works properly
void Entity::SetRigidbodyPosition(btVector3 position, btVector3 orientation)
{
		btTransform initialTransform;

		initialTransform.setOrigin(position);
		initialTransform.setRotation(btQuaternion(orientation.getX(), orientation.getY(), orientation.getY()));

		rBody->setWorldTransform(initialTransform);
		rBody->getMotionState()->setWorldTransform(initialTransform);
}

void Entity::SetRepeatTexture(float x, float y)
{
	repeatTex = XMFLOAT2(x, y);
}

void Entity::SetUVOffset(float x, float y)
{
	uvOffset = XMFLOAT2(x, y);
}

void Entity::SetShadowData(ShadowData shadowData)
{
	this->shadowData = shadowData;
}

void Entity::SetDepthStencilData(DepthStencilData depthStencilData)
{
	this->depthStencilData = depthStencilData;
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

void Entity::ToggleHBAOPlus(bool toggle)
{
	hbaoPlusEnabled = toggle;
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

void Entity::PrepareMaterialForDraw(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	SimpleVertexShader* vs = (*materialMap)[n]->GetVertexShader();
	SimplePixelShader* ps = (*materialMap)[n]->GetPixelShader();

	// Send data to shader variables
		//  - Do this ONCE PER OBJECT you're drawing
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.
	vs->SetMatrix4x4("world", eTransform.GetWorldMatrix());
	vs->SetMatrix4x4("view", view);
	vs->SetMatrix4x4("projection", proj);

	ps->SetData(
		"uvMult",
		&repeatTex,
		sizeof(repeatTex)
	);

	ps->SetData(
		"uvOffset",
		&uvOffset,
		sizeof(uvOffset)
	);

	if (shadowsEnabled) {
		for (size_t i = 0; i < shadowData.cascadeCount; i++)
		{
			string str = to_string(i);
			ps->SetShaderResourceView("ShadowMap" + str, shadowData.shadowSRV[i]);
			ps->SetMatrix4x4("shadowProj" + str, shadowData.shadowProjectionMatrix[i]);
			ps->SetFloat2("cascadeRange" + str, shadowData.widthHeight[i]);
		}
		ps->SetMatrix4x4("shadowView", shadowData.shadowViewMatrix);
		//ps->SetData("nearPlane", shadowData.nears, sizeof(float) * MAX_SHADOW_CASCADES);
		ps->SetFloat3("sunPos", shadowData.sunPos);
		ps->SetSamplerState("ShadowSampler", shadowData.shadowSampler);
	}

	if (Config::SSAOEnabled && (*materialMap)[n]->GetMaterialData().SSAO) {
		ps->SetMatrix4x4("ssaoView", view);
		ps->SetMatrix4x4("ssaoProjection", proj);
		ps->SetShaderResourceView("DepthStencilMap", depthStencilData.depthStencilSRV);
		ps->SetSamplerState("DepthStencilSampler", depthStencilData.depthStencilSampler);
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
	return name.STDStr();
}

void Entity::AddChild(Entity* child, bool preserveChild)
{
	children->push_back(child);
	child->parent = this;

	//child->eTransform.SetParent(&eTransform, preserveChild);
	eTransform.AddChild(&child->eTransform, preserveChild);
}

void Entity::AddAutoBoxCollider()
{
	if (mesh != nullptr) {
		if (mesh->HasChildren()) {
			Mesh** children = mesh->GetChildren();
			for (size_t i = 0; i < mesh->GetChildCount(); i++)
			{
				Collider* coll = new Collider(children[i], children[i]->GetVertices());
				colliders->push_back(coll);
				eTransform.AddChild(&coll->GetTransform(), false);
			}
		}
		else {
			Collider* coll = new Collider(mesh, mesh->GetVertices());
			colliders->push_back(coll);
			eTransform.AddChild(&coll->GetTransform(), false);
		}
	}
	else {
		vector<XMFLOAT3> v;
		v.push_back(XMFLOAT3(0.5f, 0.5f, 0.5f));
		v.push_back(XMFLOAT3(-0.5f, -0.5f, -0.5f));
		colliders->push_back(new Collider(nullptr, v));
	}

	colliderCnt = colliders->size();
}

bool Entity::CheckSATCollision(Entity* other)
{
	unsigned int result;
	for (size_t i = 0; i < colliders->size(); i++)
	{
		for (size_t j = 0; j < other->colliders->size(); j++)
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
	for (size_t i = 0; i < colliders->size(); i++)
	{
		for (size_t j = 0; j < other->colliders->size(); j++)
		{
			isColliding = (*colliders)[i]->CheckSATCollisionForCorrection((*other->colliders)[j], result);
			if (isColliding) break;
		}
	}
	
	if (isColliding) {
		XMVECTOR modifiedVec = XMLoadFloat3(&result);
		XMVECTOR dist;
		XMVECTOR thisPos = XMLoadFloat3(&eTransform.GetPosition());
		XMVECTOR otherPos = XMLoadFloat3(&other->eTransform.GetPosition());
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
			other->eTransform.Move(otherResult);
		}
		XMFLOAT3 modifiedResult;
		XMStoreFloat3(&modifiedResult, modifiedVec);
		eTransform.Move(modifiedResult);
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

btCollisionShape* Entity::GetBTCollisionShape(int index)
{
	if(index >= colliderCnt || index < 0) return nullptr;
	return collShape[index];
}

btCompoundShape* Entity::GetBTCompoundShape(int index)
{
	if(compoundShape == nullptr || index >= colliderCnt || index < 0) return nullptr;
	return compoundShape;
}

void Entity::Update()
{
	//for (size_t i = 0; i < colliders->size(); i++)
	//{
	//	(*colliders)[i]->Update(eTransform.GetWorldMatrix());
	//}
}

float Entity::GetMass()
{
	return mass;
}

bool Entity::HasTag(string tag)
{
	for (size_t i = 0; i < tagCount; i++)
	{
		if (tags[i] == tag) return true;
	}
	return false;
}

bool Entity::AddTag(string tag)
{
	if (tagCount < MAX_ENTITY_TAG_COUNT) {
		tags[tagCount++] = tag;
		return true;
	}
	return false;
}

bool Entity::RemoveTag(string tag)
{
	for (size_t i = 0; i < tagCount; i++)
	{
		if (tags[i] == tag) {
			tags[i] = tags[tagCount - 1];
			tags[tagCount - 1] = "";
			tagCount--;
			return true;
		}
	}
	return false;
}

EEString<EESTRING_SIZE>* Entity::GetTags()
{
	return tags;
}

unsigned int Entity::GetTagCount()
{
	return tagCount;
}

bool Entity::HasLayer(string layer)
{
	for (size_t i = 0; i < layerCount; i++)
	{
		if (layers[i] == layer) return true;
	}
	return false;
}

bool Entity::AddLayer(string layer)
{
	if (layerCount < MAX_ENTITY_LAYER_COUNT && Config::EntityLayers.count(layer)) {
		layers[layerCount++] = layer;
		layerMask |= Config::EntityLayers[layer];
		return true;
	}
	return false;
}

bool Entity::RemoveLayer(string layer)
{
	if (Config::EntityLayers.count(layer)) {
		for (size_t i = 0; i < layerCount; i++)
		{
			if (layers[i] == layer) {
				layers[i] = layers[layerCount - 1];
				layers[layerCount - 1] = "";
				layerCount--;
				layerMask ^= Config::EntityLayers[layer];
				return true;
			}
		}
	}
	return false;
}

EEString<EESTRING_SIZE>* Entity::GetLayers()
{
	return layers;
}

unsigned int Entity::GetLayerCount()
{
	return layerCount;
}

unsigned int Entity::GetLayerMask()
{
	return layerMask;
}

void Entity::RemoveFromPhysicsSimulation()
{
	if (Config::DynamicsWorld != nullptr) {
		delete rBody->getMotionState(); 
		rBody->setMotionState(nullptr); // set motion state to nullptr
		Config::DynamicsWorld->removeRigidBody(rBody);
	}
}

void Entity::EmptyEntity()
{
	if (Config::DynamicsWorld != nullptr) {
		Config::DynamicsWorld->removeCollisionObject(rBody);
	}
		
	this->isEmptyObj = true;
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
		colliders->clear();
	}

	if (Config::DynamicsWorld != nullptr && rBody != nullptr)
	{
		Config::DynamicsWorld->removeCollisionObject(rBody);
		delete rBody->getMotionState();
		delete rBody;
	}

	if (collShape != nullptr) {
		for (size_t i = 0; i < colliderCnt; i++)
		{
			delete collShape[i];
		}
		delete[] collShape;
	}

	if (compoundShape != nullptr)
		delete compoundShape;

	if (materialMap != nullptr) {
		materialMap->clear();
		delete materialMap;
	}

	if (children != nullptr) {
		children->clear();
		delete children;
	}
	if (colliders != nullptr)
		delete colliders;

}


