#pragma once

enum class PHYSICS_WRAPPER_TYPE {
	ENTITY,
	PARTICLE,
};

struct PhysicsWrapper {
	PHYSICS_WRAPPER_TYPE type;
	void* objectPointer;
	void (*callback)(void* objPointer);
};