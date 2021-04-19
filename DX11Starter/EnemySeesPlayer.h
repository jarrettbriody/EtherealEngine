#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"
#include <math.h>

class EnemySeesPlayer : public Behavior
{
private:
    Entity* enemy;
    Entity* player;
    float viewAngle;
    float viewDistance;
public:
    EnemySeesPlayer(Entity* e, Entity* p, float angle, float distance) : enemy(e), player(p), viewAngle(angle), viewDistance(distance) {}

    ~EnemySeesPlayer() {}

    void OnInitialize();

    void OnTerminate(Status s);

    Status Update();

    XMFLOAT3 CalculateEnemyForward();

    XMVECTOR PointInsideTriangle(XMVECTOR p, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2);
};