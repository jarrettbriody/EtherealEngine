#pragma once
#include "pch.h"

template <unsigned int N>
class EEString {
private:
	char buffer[N];
	unsigned int bufferSize = N;
	unsigned int count = 0;
public:
	EEString() { bufferSize = N; };
	EEString(const EEString& other) {
		bufferSize = N;
		unsigned int size = (other.count > bufferSize) ? bufferSize : other.count;
		memcpy(buffer, other.buffer, other.count);
		count = other.count;
	};
	EEString(std::string str) {
		bufferSize = N;
		unsigned int size = (str.size() > bufferSize) ? bufferSize : str.size();
		memcpy(buffer, str.c_str(), size);
		count = size;
	};
	~EEString() {

	};
	void operator= (const EEString& other) {
		bufferSize = N;
		unsigned int size = (other.count > bufferSize) ? bufferSize : other.count;
		memcpy(buffer, other.buffer, other.count);
		count = other.count;
	};
	void operator= (const std::string& other) {
		bufferSize = N;
		unsigned int size = (other.size() > bufferSize) ? bufferSize : other.size();
		memcpy(buffer, other.c_str(), size);
		count = size;
	};
	bool operator== (const EEString& other) {
		if (count != other.count) return false;
		for (size_t i = 0; i < count; i++)
		{
			if (buffer[i] != other.buffer[i]) return false;
		}
		return true;
	};
	bool operator== (const std::string& other) {
		if (count != other.size()) return false;
		for (size_t i = 0; i < count; i++)
		{
			if (buffer[i] != other[i]) return false;
		}
		return true;
	};
	std::string STDStr () {
		return std::string(buffer, count);
	};
};