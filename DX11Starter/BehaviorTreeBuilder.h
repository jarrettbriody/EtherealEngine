#pragma once
#include "pch.h"
#include "BehaviorTree.h"

template <class parent_type, class behavior_type>
class LeafBuilder
{
private:
	parent_type* parentBuilder;
	behavior_type* leafBehavior;
public:
	LeafBuilder(parent_type* parent, behavior_type* behavior) : parentBuilder(parent), leafBehavior(behavior) {}
	~LeafBuilder(void) = default;

	template <typename ret, typename obj, typename... param_types>
	LeafBuilder& Invoke(ret(obj::* func)(param_types...), param_types... params)
	{
		leafBehavior->*func(params...);

		return *this;
	}

	parent_type& End(void)
	{
		return *parentBuilder;
	}
};

template <class parent_type>
class DecoratorBuilder;

template <class parent_type>
class CompositeBuilder
{
private:
	parent_type* parentBuilder;
	Composite* compositeBehavior;
public:
	CompositeBuilder(parent_type* parent, Composite* composite) : parentBuilder(parent), compositeBehavior(composite) {}
	~CompositeBuilder(void) = default;

	template <class behavior_type, typename... param_types>
	LeafBuilder<CompositeBuilder, behavior_type> Leaf(param_types... params)
	{
		behavior_type* behavior = new behavior_type(params...);
		compositeBehavior->AddChild(behavior);

		return LeafBuilder<CompositeBuilder<parent_type>, behavior_type>(this, behavior);
	}

	template <class composite_type, typename... param_types>
	CompositeBuilder<CompositeBuilder<parent_type>> Composite(param_types... params)
	{
		composite_type* composite = new composite_type(params...);
		compositeBehavior->AddChild(composite);

		return CompositeBuilder<CompositeBuilder<parent_type>>(this, composite);
	}

	template <class decorator_type, typename... param_types>
	DecoratorBuilder<CompositeBuilder> Decorator(param_types... params)
	{
		decorator_type* decorator = new decorator_type(nullptr, params...);
		compositeBehavior->AddChild(decorator);

		return DecoratorBuilder<CompositeBuilder<parent_type>>(this, decorator);
	}

	parent_type& End(void)
	{
		return *parentBuilder;
	}
};

template <class parent_type>
class DecoratorBuilder
{
private:
	parent_type* parentBuilder;
	Decorator* decoratorBehavior;
public:
	DecoratorBuilder(parent_type* parent, Decorator* decorator) : parentBuilder(parent), decoratorBehavior(decorator) {}
	~DecoratorBuilder(void) = default;

	template <class behavior_type, typename... param_types>
	LeafBuilder<DecoratorBuilder, behavior_type> Leaf(param_types... params)
	{
		behavior_type* behavior = new behavior_type(params...);
		decoratorBehavior->SetChild(behavior);

		return LeafBuilder<DecoratorBuilder<parent_type>, behavior_type>(this, behavior);
	}

	template <class composite_type, typename... param_types>
	CompositeBuilder<DecoratorBuilder> Composite(param_types... params)
	{
		composite_type* composite = new composite_type(params...);
		decoratorBehavior->SetChild(composite);

		return CompositeBuilder<DecoratorBuilder<parent_type>>(this, composite);
	}

	template <class decorator_type, typename... param_types>
	DecoratorBuilder<DecoratorBuilder> Decorator(param_types... params)
	{
		decorator_type* decorator = new decorator_type(nullptr, params...);
		decoratorBehavior->SetChild(decorator);

		return DecoratorBuilder<DecoratorBuilder<parent_type>>(this, decorator);
	}

	parent_type& End(void)
	{
		return *parentBuilder;
	}
};

class BehaviorTreeBuilder
{
private:
	Behavior* root;
public:
	BehaviorTreeBuilder(void) : root(nullptr) {}
	~BehaviorTreeBuilder(void) = default;

	template <class behavior_type, typename... param_types>
	LeafBuilder<BehaviorTreeBuilder, behavior_type> Leaf(param_types... params)
	{
		behavior_type* behavior = new behavior_type(params...);
		root = behavior;

		return LeafBuilder<BehaviorTreeBuilder, behavior_type>(this, node);
	}

	template <class composite_type, typename... param_types>
	CompositeBuilder<BehaviorTreeBuilder> Composite(param_types... params)
	{
		composite_type* composite = new composite_type(params...);
		root = composite;

		return CompositeBuilder<BehaviorTreeBuilder>(this, composite);
	}

	template <class decorator_type, typename... param_types>
	DecoratorBuilder<BehaviorTreeBuilder> Decorator(param_types... params)
	{
		decorator_type* decorator = new decorator_type(nullptr, params...);
		root = decorator;

		return DecoratorBuilder<BehaviorTreeBuilder>(this, decorator);
	}

	BehaviorTree* End(void)
	{
		return new BehaviorTree(root);
	}
};