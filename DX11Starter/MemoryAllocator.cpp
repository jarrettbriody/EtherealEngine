#include "MemoryAllocator.h"

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

bool MemoryAllocator::CreatePool(unsigned int pool, unsigned int size, unsigned int blockSize)
{
	if (pool > maxPools || pool < 0)
		return false;
	if (pools[pool].startPtr != nullptr)
		return false;

	unsigned int alignedSize = size - (size % alignment);
	unsigned int alignedBlockSize = blockSize + (alignment - (blockSize % alignment));
	if (alignedBlockSize - blockSize < 8) alignedBlockSize += alignment;

	if (usedMemory >= this->size || usedMemory + alignedSize >= this->size) 
		return false;

	pools[poolCnt] = { (char*)currentPtr, alignedSize, alignedBlockSize, blockSize, 0, (char*)currentPtr };
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

void* MemoryAllocator::AllocateToPool(unsigned int pool, unsigned int memorySize, bool& success)
{
	if (pool > maxPools || pool < 0)
		return false;

	if (pools[pool].startPtr == nullptr)
		return false;

	if (pools[pool].usedMemory >= pools[pool].size) {
		success = false;
		return nullptr;
	}

	char* writeMemoryLoc = (char*)pools[pool].currentPtr + (pools[pool].blockSize - memorySize);
	//memcpy(writeMemoryLoc, memoryLocation, memorySize);
	uintptr_t castedPtr;
	memcpy(&castedPtr, pools[pool].currentPtr, sizeof(char*));
	void* castedPtrToRetrieve = reinterpret_cast<void*>(castedPtr);
	//memcpy(pools[pool].currentPtr, *pools[pool].currentPtr, sizeof(char*));
	//char* newPtr = *pools[pool].currentPtr;
	//*pools[pool].currentPtr = **pools[pool].currentPtr;
	pools[pool].currentPtr = castedPtrToRetrieve;
	pools[pool].usedMemory += pools[pool].blockSize;
	success = true;

	return (void*)writeMemoryLoc;
}

bool MemoryAllocator::DeallocateFromPool(unsigned int pool, void* memoryLocation, unsigned int memorySize)
{
	if (pool > 9 || pool < 0)
		return false;

	char* reqMem = (char*)memoryLocation;

	if (pools[pool].startPtr == nullptr)
		return false;

	if (reqMem < (char*)pools[pool].startPtr || reqMem > (char*)pools[pool].startPtr + (pools[pool].size - pools[pool].storedMemorySize))
		return false;

	char* actualMemoryLoc = reqMem - (pools[pool].blockSize - memorySize);
	uintptr_t castedPtrToStore = reinterpret_cast<uintptr_t>(&pools[pool].currentPtr);
	memcpy(actualMemoryLoc, &castedPtrToStore, sizeof(char*));
	pools[pool].currentPtr = actualMemoryLoc;

	return true;
}
