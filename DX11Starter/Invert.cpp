#include "pch.h"
#include "Invert.h"

void Invert::OnInitialize()
{
}

void Invert::OnTerminate(Status s)
{
}

Status Invert::Update()
{
	child->Tick();
	
	if (child->GetStatus() == SUCCESS)
		return FAILURE;
	else if (child->GetStatus() == FAILURE)
		return SUCCESS;

	return INVALID;
}