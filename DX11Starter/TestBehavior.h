#pragma once
#include "pch.h"
#include "BehaviorTree.h"

class TestBehavior : public Behavior
{
public:
    int initializeCalled;
    int terminateCalled;
    int updateCalled;
    Status returnStatus;
    Status terminateStatus;

    TestBehavior()
        : initializeCalled(0)
        , terminateCalled(0)
        , updateCalled(0)
        , returnStatus(RUNNING)
        , terminateStatus(INVALID)
    {
    }

    virtual ~TestBehavior() { }

    virtual void OnInitialize()
    {
        ++initializeCalled;
    }

    virtual void OnTerminate(Status s)
    {
        ++terminateCalled;
        terminateStatus = s;
    }

    virtual Status Update()
    {
        ++updateCalled;
        return returnStatus;
    }
};