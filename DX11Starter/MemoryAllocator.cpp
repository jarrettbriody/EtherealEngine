#include "pch.h"
#include "MemoryAllocator.h"

MemoryAllocator* MemoryAllocator::instance = nullptr;

MemoryAllocator::MemoryAllocator(unsigned int size, unsigned int alignment, unsigned int maxPools)
{
	originalBufferPtr = memoryBuffer = malloc(((size_t)size + alignment) - 1);
	const uintptr_t address = reinterpret_cast<uintptr_t>(memoryBuffer);
	const size_t mask = alignment - 1;
	//assert((alignment & mask) == 0);
	const uintptr_t alignedAddress = (address + mask) & ~mask;
	memoryBuffer = currentPtr = reinterpret_cast<void*>(alignedAddress);
	this->size = size;
	this->alignment = alignment;
	this->maxPools = maxPools;
	pools = new Pool[maxPools];
}

MemoryAllocator::~MemoryAllocator()
{
	delete[] pools;
	free(originalBufferPtr);
}

bool MemoryAllocator::SetupInstance(unsigned int size, unsigned int alignment, unsigned int maxPools)
{
	if (instance == nullptr) {
		instance = new MemoryAllocator(size, alignment, maxPools);
		return true;
	}
	return false;
}

MemoryAllocator* MemoryAllocator::GetInstance()
{
	return instance;
}

bool MemoryAllocator::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
}

bool MemoryAllocator::CreatePool(unsigned int pool, unsigned int size, unsigned int slugSize)
{
	if (pool > maxPools || pool < 0)
		return false;
	if (pools[pool].startPtr != nullptr)
		return false;

	unsigned int alignedSize = size - (size % alignment);
	unsigned int alignedBlockSize = slugSize + (alignment - (slugSize % alignment));
	if (alignedBlockSize - slugSize < 8) alignedBlockSize += alignment;

	if (usedMemory >= this->size || usedMemory + alignedSize >= this->size) 
		return false;

	pools[poolCnt] = { (char*)currentPtr, alignedSize, alignedBlockSize, slugSize, 0, (char*)currentPtr };
	currentPtr = (void*)((char*)currentPtr + alignedSize);
	usedMemory += alignedSize;

	unsigned int blockCount = alignedSize / alignedBlockSize;
	char* traversingPtr = (char*)pools[poolCnt].startPtr;
	char* nextTraversingPtr = traversingPtr;
	uintptr_t castedPtrToStore;// = reinterpret_cast<uintptr_t>(traversingPtr);
	//memcpy(traversingPtr, &castedPtrToStore, sizeof(char*));
	for (size_t i = 0; i < blockCount - 1; i++)
	{
		nextTraversingPtr = nextTraversingPtr + alignedBlockSize;
		castedPtrToStore = reinterpret_cast<uintptr_t>(nextTraversingPtr);
		memcpy(traversingPtr, &castedPtrToStore, sizeof(char*));
		traversingPtr = nextTraversingPtr;
	}
	castedPtrToStore = reinterpret_cast<uintptr_t>(pools[poolCnt].startPtr);
	memcpy(traversingPtr, &castedPtrToStore, sizeof(char*));

	poolCnt++;

	return true;
}

void* MemoryAllocator::AllocateToPool(unsigned int pool, unsigned int slugSize, bool& success)
{
	//check that params are valid
	if (pool > maxPools || pool < 0 || 
		pools[pool].startPtr == nullptr ||
		pools[pool].usedMemory >= pools[pool].size) {
		success = false;
		return nullptr;
	}

	//calc actual memory slug location
	char* writeMemoryLoc = (char*)pools[pool].currentPtr + (pools[pool].blockSize - slugSize);

	//de-encode encoded linked list pointer in memory block
	uintptr_t castedPtr;
	memcpy(&castedPtr, pools[pool].currentPtr, sizeof(char*));
	void* castedPtrToRetrieve = reinterpret_cast<void*>(castedPtr);

	//traverse to next memory block location
	pools[pool].currentPtr = castedPtrToRetrieve;

	//count mem usage
	pools[pool].usedMemory += pools[pool].blockSize;
	success = true;

	return (void*)writeMemoryLoc;
}

bool MemoryAllocator::DeallocateFromPool(unsigned int pool, void* memoryLocation, unsigned int slugSize)
{
	char* reqMem = (char*)memoryLocation;

	//check that params are valid
	if (pool > maxPools || pool < 0 ||
		pools[pool].startPtr == nullptr ||
		reqMem < (char*)pools[pool].startPtr || 
		reqMem > (char*)pools[pool].startPtr + (pools[pool].size - pools[pool].slugSize)) {
		return false;
	}

	char* actualMemoryLoc = reqMem - (pools[pool].blockSize - slugSize);
	uintptr_t castedPtrToStore = reinterpret_cast<uintptr_t>(pools[pool].currentPtr);
	memcpy(actualMemoryLoc, &castedPtrToStore, sizeof(char*));
	pools[pool].currentPtr = actualMemoryLoc;

	return true;
}
