#include "FPSController.h"

void FPSController::Init()
{
	cam = ScriptManager::renderer->GetCamera("main");
	direction = cam->direction; 

	// TODO: Easier setting of physics characteristics via Bullet (coll shape, mass, restitution, other properties)
	
	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->activate();
	// playerRBody->setUserPointer("Player");

	// TODO: Remove from physics world to change mass -- m_PhysicsWorld->removeRigidBody( bt->body() );

	/*btVector3 inertia;
	playerRBody->getCollisionShape()->calculateLocalInertia(btScalar(1), inertia);
	playerRBody->setMassProps(btScalar(1), inertia);*/

	// TODO: Add back to physics world with updated mass -- m_PhysicsWorld->addRigidBody( bt->body() );
}

void FPSController::Update()
{
	// TODO: Figure out how much of the following is still needed
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction); 
	dir.m128_f32[1] = 0.0f;
	XMVECTOR right = XMVector3Cross(dir, XMLoadFloat3(&yAxis));

	Move();

	// update camera pos from player physics transform
	btTransform playerTransform;
	playerRBody->getMotionState()->getWorldTransform(playerTransform);
	btVector3 playerTransformPos = playerTransform.getOrigin();

	cam->SetPosition(XMFLOAT3(playerTransformPos.getX(), playerTransformPos.getY() /*Offset for head height*/, playerTransformPos.getZ()));
}

void FPSController::Move()
{
	playerRBody->activate();

	btVector3 linearVelocity;
	btVector3 moveVec;

	if (GetAsyncKeyState('W') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(0.0f, 0.0f, 10.0f);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(0.0f, 0.0f, -10.0f);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(-10.0f, 0.0f, 0.0f);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(10.0f, 0.0f, 0.0f);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		linearVelocity = btVector3(playerRBody->getLinearVelocity().getX(), 0.0f, playerRBody->getLinearVelocity().getZ());
		moveVec = btVector3(0.0f, 15.0f, 0.0f);
	}

	playerRBody->setLinearVelocity(linearVelocity);
	playerRBody->applyCentralImpulse(moveVec);
}

void FPSController::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001) {
		cam->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

/*
void SceneManager::CheckCollisionWithFloor()
{
	int numManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto* obA = contactManifold->getBody0();
		auto* obB = contactManifold->getBody1();

		if (obA->getUserPointer() == "Player" && obB->getUserPointer() == "Floor")
		{
			int numContacts = contactManifold->getNumContacts();
			if (numContacts > 0 && !doubleJumpControl)
			{
				jumpCount = 0;
				doubleJumpControl = false;
			}
		}
	}
}
*/
