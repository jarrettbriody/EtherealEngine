#pragma once
#include "pch.h"

enum Status
{
    INVALID,
    SUCCESS,
    FAILURE,
    RUNNING,
    ABORTED,
};

//-------------------------------------------------------

class Behavior
{
private:
    Status currentStatus;
public:
    virtual Status Update() = 0;
    virtual void OnInitialize() {}
    virtual void OnTerminate(Status) {}

    Behavior() : currentStatus(INVALID) {}
    virtual ~Behavior() {}

    Status Tick()
    {
        if (currentStatus != RUNNING)
            OnInitialize();

        currentStatus = Update();

        if (currentStatus != RUNNING)
            OnTerminate(currentStatus);

        return currentStatus;
    }

    void Reset()
    {
        currentStatus = INVALID;
    }

    void Abort()
    {
        OnTerminate(ABORTED);
        currentStatus = ABORTED;
    }

    bool IsTerminated() const
    {
        return currentStatus == SUCCESS || currentStatus == FAILURE;
    }

    bool IsRunning() const
    {
        return currentStatus == RUNNING;
    }

    Status GetStatus() const
    {
        return currentStatus;
    }
};

//-------------------------------------------------------

class Decorator : public Behavior
{
protected:
    Behavior* child;

public:
    Decorator(Behavior* childBehavior) : child(childBehavior) {}
    ~Decorator() { delete child; }
    void SetChild(Behavior* behavior)
    {
        child = behavior;
    }
};

//-------------------------------------------------------

class Repeat : public Decorator
{
protected:
    int limit;
    int counter;

public:
    Repeat(Behavior* childBehavior) : Decorator(childBehavior) {}

    void SetCount(int count)
    {
        limit = count;
    }

    void OnInitialize()
    {
        counter = 0;
    }

    Status Update()
    {
        for (;;)
        {
            child->Tick();
            if (child->GetStatus() == RUNNING) break;
            if (child->GetStatus() == FAILURE) return FAILURE;
            if (++counter == limit) return SUCCESS;
            child->Reset();
        }
        return INVALID;
    }
};

//-------------------------------------------------------

class Composite : public Behavior
{
protected:
    typedef std::vector<Behavior*> Behaviors;
    Behaviors children;
public:
    ~Composite() {
        for (size_t i = 0; i < children.size(); i++)
        {
            delete children[i];
        }
    }
    void AddChild(Behavior* child)
    {
        children.push_back(child);
    }

    void RemoveChild(Behavior* child)
    {
        children.erase(std::find(children.begin(), children.end(), child));
    }

    void ClearChildren()
    {
        children.clear();
    }
};

//-------------------------------------------------------

class Sequence : public Composite
{
protected:
    Behaviors::iterator currentChild;

    virtual ~Sequence() {}

    virtual void OnInitialize()
    {
        currentChild = children.begin();
    }

    virtual Status Update()
    {
        for (;;)
        {
            Status s = (*currentChild)->Tick();

            if (s != SUCCESS)
                return s;

            if (++currentChild == children.end())
                return SUCCESS;
        }
    }
};

//-------------------------------------------------------

class Selector : public Composite
{
protected:
    Behaviors::iterator current;
    
    virtual ~Selector() {}

    virtual void OnInitialize()
    {
        current = children.begin();
    }

    virtual Status Update()
    {
        for (;;)
        {
            Status s = (*current)->Tick();

            if (s != FAILURE)
                return s;

            if (++current == children.end())
                return FAILURE;
        }
    }
};

//-------------------------------------------------------

class Parallel : public Composite
{
public:
    enum Policy
    {
        RequireOne,
        RequireAll,
    };

    Parallel(Policy forSuccess, Policy forFailure) : successPolicy(forSuccess), failurePolicy(forFailure) {}
    virtual ~Parallel() {}
protected:
    Policy successPolicy;
    Policy failurePolicy;

    virtual Status Update()
    {
        size_t successCount = 0, failureCount = 0;

        for (Behaviors::iterator it = children.begin(); it != children.end(); ++it)
        {
            Behavior& b = **it;
            if (!b.IsTerminated())
                b.Tick();

            if (b.GetStatus() == SUCCESS)
            {
                ++successCount;
                if (successPolicy == RequireOne)
                    return SUCCESS;
            }

            if (b.GetStatus() == FAILURE)
            {
                ++failureCount;
                if (failurePolicy == RequireOne)
                    return FAILURE;
            }
        }

        if (failurePolicy == RequireAll && failureCount == children.size())
            return FAILURE;

        if (successPolicy == RequireAll && successCount == children.size())
            return SUCCESS;

        return RUNNING;
    }

    virtual void OnTerminate(Status)
    {
        for (Behaviors::iterator it = children.begin(); it != children.end(); ++it)
        {
            Behavior& b = **it;
            if (b.IsRunning())
                b.Abort();
        }
    }
};

//-------------------------------------------------------

class Monitor : public Parallel
{
public:
    Monitor() : Parallel(Parallel::RequireOne, Parallel::RequireOne) {}

    void AddCondition(Behavior* condition)
    {
        children.insert(children.begin(), condition);
    }

    void AddAction(Behavior* action)
    {
        children.push_back(action);
    }
};

//-------------------------------------------------------

class ActiveSelector : public Selector
{
protected:
    virtual void OnInitialize()
    {
        current = children.end();
    }

    virtual Status Update()
    {
        Behaviors::iterator previous = current;

        Selector::OnInitialize();
        Status result = Selector::Update();

        if (previous != children.end() && current != previous)
            (*previous)->OnTerminate(ABORTED);

        return result;
    }
};

//-------------------------------------------------------

class BehaviorTree
{
private:
    Behavior* root;
public:
    BehaviorTree(Behavior* rootBehavior) : root(rootBehavior) {}
    ~BehaviorTree() { delete root; }

    Status Run()
    {
        return root->Tick();
    }
};

//-------------------------------------------------------
// TESTS
//-------------------------------------------------------

struct MockBehavior : public Behavior
{
    int initializeCalled;
    int terminateCalled;
    int updateCalled;
    Status returnStatus;
    Status terminateStatus;

    MockBehavior()
        : initializeCalled(0)
        , terminateCalled(0)
        , updateCalled(0)
        , returnStatus(RUNNING)
        , terminateStatus(INVALID)
    {
    }

    virtual ~MockBehavior() { }

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