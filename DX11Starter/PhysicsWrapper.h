#pragma once

enum class PHYSICS_WRAPPER_TYPE {
	ENTITY,
	PARTICLE,
};

struct PhysicsWrapper {
	PHYSICS_WRAPPER_TYPE type = PHYSICS_WRAPPER_TYPE::ENTITY;
	void* objectPointer = nullptr;
	void (*callback)(void* objPointer) = nullptr;
};