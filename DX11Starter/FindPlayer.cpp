#include "pch.h"
#include "FindPlayer.h"

void FindPlayer::OnInitialize()
{
}

void FindPlayer::OnTerminate(Status s)
{
}

Status FindPlayer::Update()
{
	if (path->size() != 0)
		return SUCCESS;

	*path = aStarSolver->FindPath(enemy->GetPosition(), player->GetPosition());

	if (path->size() == 0)
		return FAILURE;

	return SUCCESS;
}