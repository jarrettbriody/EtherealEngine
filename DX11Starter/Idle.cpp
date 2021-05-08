#include "pch.h"
#include "Idle.h"

void Idle::OnInitialize()
{
}

void Idle::OnTerminate(Status s)
{
}

Status Idle::Update()
{
	if (*inCombat)
		*inCombat = false;

	currentSpeed = enemy->GetRBody()->getLinearVelocity().length();
	
	if (currentSpeed > 0.001f)
	{
		directionVector = enemy->GetTransform().GetDirectionVector();
		movementDirection = btVector3(directionVector.x, directionVector.y, directionVector.z);
		currentSpeed = enemy->GetRBody()->getLinearVelocity().length() * dampingScalar;

		// FORCES ADDED TO RIGIDBODY
		enemy->GetRBody()->activate();
		enemy->GetRBody()->setLinearVelocity(movementDirection.normalized() * currentSpeed);
	}

	return SUCCESS;
}