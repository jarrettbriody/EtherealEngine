#include "pch.h"
#include "AbilityAvailable.h"

void AbilityAvailable::OnInitialize()
{
}

void AbilityAvailable::OnTerminate(Status s)
{
}

Status AbilityAvailable::Update()
{
	if (*coolDownTimer <= 0)
	{
		return SUCCESS;
	}

	return FAILURE;
}
