#include "pch.h"
#include "InCombat.h"

void InCombat::OnInitialize()
{
}

void InCombat::OnTerminate(Status s)
{
}

Status InCombat::Update()
{
	if (*inCombat)
		return SUCCESS;

	return FAILURE;
}