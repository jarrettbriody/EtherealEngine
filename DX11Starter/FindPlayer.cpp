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

	*path = grid->FindPath(enemy->GetPosition(), player->GetPosition());

	if (path->size() == 0)
		return FAILURE;

	if (path->size() == 2)
	{
		cout << "Enemy pos- X: " << enemy->GetPosition().x << " | Z: " << enemy->GetPosition().z << endl;
		cout << "Node 1- X: " << path->front()->GetPos().x << " | Z: " << path->front()->GetPos().z << endl;
		cout << "Node 2- X: " << path->back()->GetPos().x << " | Z: " << path->back()->GetPos().z << endl;
	}

	return SUCCESS;
}