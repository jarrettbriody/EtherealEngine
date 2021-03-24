#pragma once
#include "KeyboardEvent.h"
#include <queue>

class Keyboard
{
public:
	Keyboard();
	bool KeyIsPressed(const unsigned char keycode);
	bool OnKeyDown(const unsigned char keycode);
	bool OnKeyUp(const unsigned char keycode);
	bool CheckKeysPressed(const unsigned char keycode[], int arrLength);
	bool NoKeyDown();
	bool KeyBufferIsEmpty();
	void RegisterKeyPress(const unsigned char key);
	void RegisterKeyRelease(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	bool IsKeysAutoRepeat();

private:
	bool autoRepeatKeys = false;
	bool keyStates[256];
	std::unordered_map<unsigned char, KeyboardEvent> keyBuffer;

};