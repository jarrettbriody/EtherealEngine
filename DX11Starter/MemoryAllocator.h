#pragma once
#include "pch.h"

struct Pool{
	void* startPtr = nullptr;
	unsigned int size = 0;
	unsigned int blockSize = 0;
	unsigned int slugSize = 0;
	unsigned int usedMemory = 0;
	void* currentPtr = nullptr;
};

class MemoryAllocator
{
private:
	static MemoryAllocator* instance;

	void* memoryBuffer = nullptr;
	void* originalBufferPtr = nullptr;
	void* currentPtr = nullptr;
	unsigned int alignment = 0;
	unsigned int size = 0;
	Pool* pools = nullptr;
	unsigned int poolCnt = 0;
	unsigned int maxPools = 0;
	unsigned int usedMemory = 0;
	MemoryAllocator(unsigned int size, unsigned int alignment, unsigned int maxPools = 10);
	~MemoryAllocator();
public:
	static bool SetupInstance(unsigned int size, unsigned int alignment, unsigned int maxPools = 10);
	static MemoryAllocator* GetInstance();
	static bool DestroyInstance();
	bool CreatePool(unsigned int pool, unsigned int size, unsigned int slugSize);
	void* AllocateToPool(unsigned int pool, unsigned int slugSize, bool& success);
	bool DeallocateFromPool(unsigned int pool, void* memoryLocation, unsigned int slugSize);
};

