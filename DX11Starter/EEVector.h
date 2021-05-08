#pragma once
#include "pch.h"

template <class T>
class EEVector {
private:
	T* buffer = nullptr;
	unsigned int bufferSize = 0;
	unsigned int count = 0;

public:
	EEVector(unsigned int preAlloc = 16) {
		bufferSize = preAlloc;
		buffer = new T[bufferSize];
		ZeroMemory(buffer, sizeof(T) * bufferSize);
	};

	~EEVector() {
		delete[] buffer;
	};

	void operator= (const EEVector<T>& other) {
		bufferSize = other.bufferSize;
		count = other.count;
		buffer = new T[bufferSize];
		memcpy(buffer, other.buffer, sizeof(T) * other.bufferSize);
	};

	T& operator[] (const unsigned int index) {
		return buffer[index];
	};

	unsigned int Count() {
		return count;
	};

	unsigned int BufferSize() {
		return bufferSize;
	};

	void Clear() {
		ZeroMemory(buffer, sizeof(T) * bufferSize);
		count = 0;
	};

	void Push(T data) {
		if (count >= bufferSize) {
			T* oldBuffer = buffer;
			T* newBuffer = new T[(size_t)bufferSize * 2];
			memcpy(newBuffer, oldBuffer, sizeof(T) * bufferSize);
			bufferSize *= 2;
			buffer = newBuffer;
			delete[] oldBuffer;
		}
		buffer[count++] = data;
	};

	T Pop() {
		return buffer[--count];
	};

	T Remove(unsigned int index = 0) {
		T data = buffer[index];
		buffer[index] = buffer[--count];
		return data;
	};
};
