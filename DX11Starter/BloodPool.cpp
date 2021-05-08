#include "pch.h"
#include "BloodPool.h"

void BloodPool::Init()
{
	poolGhostObject = new btGhostObject();
	poolGhostObject->setCollisionFlags(poolGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	poolGhostObject->setCollisionShape(new btBoxShape(btVector3(finalScale.x / 2, 2.5f, finalScale.z / 2))); // setting y half-extent to be high enough so there are never issues with picking up blood

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Utility::Float3ToBulletVector(entity->GetTransform().GetPosition()));
	btQuaternion qx = btQuaternion(btVector3(1.0f, 0.0f, 0.0f), 0.0f);
	btQuaternion qy = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), 0.0f);
	btQuaternion qz = btQuaternion(btVector3(0.0f, 0.0f, 1.0f), 0.0f);
	btQuaternion res = qz * qy * qx;
	transform.setRotation(res);

	poolGhostObject->setWorldTransform(transform);

	entityWrapper = { PHYSICS_WRAPPER_TYPE::ENTITY, (void*)entity };
	poolGhostObject->setUserPointer(&entityWrapper);

	Config::DynamicsWorld->addCollisionObject(poolGhostObject);
}

void BloodPool::Update()
{
	// lerping the scale
	XMFLOAT3 currentScale = entity->GetTransform().GetScale();
	XMFLOAT3 newScale;
	XMStoreFloat3(&newScale, XMVectorLerp(XMLoadFloat3(&entity->GetTransform().GetScale()), XMLoadFloat3(&finalScale), growthScalar * deltaTime));

	entity->GetTransform().SetScale(newScale);
}

void BloodPool::OnCollision(btCollisionObject* other)
{
	
}

BloodPool::~BloodPool()
{
	Config::DynamicsWorld->removeCollisionObject(poolGhostObject);

	delete poolGhostObject->getCollisionShape();
	delete poolGhostObject;
}
