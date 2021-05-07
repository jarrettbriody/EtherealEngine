#include "pch.h"
#include "PlayerIsInRange.h"

void PlayerIsInRange::OnInitialize()
{
}

void PlayerIsInRange::OnTerminate(Status s)
{
}

Status PlayerIsInRange::Update()
{
	if (*inRange)
	{
		return SUCCESS;
	}

	return FAILURE;
}
