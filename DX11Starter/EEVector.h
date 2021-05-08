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
		this->buffer = nullptr;
		this->count = 0;
		bufferSize = preAlloc;
	};

	void operator= (const EEVector<T>& other) {
		bufferSize = other.bufferSize;
		count = other.count;
		this->buffer = other.buffer;
	};

	T& operator[] (const unsigned int index) {
		return this->buffer[index];
	};

	void InitBuffer() {
		if (this->buffer == nullptr) {
			this->buffer = new T[bufferSize];
			ZeroMemory(this->buffer, sizeof(T) * bufferSize);
		}
	};

	void Cleanup() {
		if (this->buffer != nullptr) {
			delete[] this->buffer;
		}
	};

	unsigned int Count() {
		return count;
	};

	unsigned int BufferSize() {
		return bufferSize;
	};

	void Clear() {
		ZeroMemory(this->buffer, sizeof(T) * bufferSize);
		count = 0;
	};

	void Push(T data) {
		if (this->buffer == nullptr) InitBuffer();
		if (count >= bufferSize) {
			T* oldBuffer = this->buffer;
			T* newBuffer = new T[(size_t)bufferSize * 2];
			memcpy(newBuffer, oldBuffer, sizeof(T) * bufferSize);
			bufferSize *= 2;
			this->buffer = newBuffer;
			delete[] oldBuffer;
		}
		this->buffer[count++] = data;
	};

	T Pop() {
		return buffer[--count];
	};

	T Remove(unsigned int index = 0) {
		T data = this->buffer[index];
		this->buffer[index] = this->buffer[--count];
		return data;
	};
};
