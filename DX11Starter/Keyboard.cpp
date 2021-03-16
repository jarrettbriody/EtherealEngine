#include "pch.h"
#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i < 256; i++)
	{
		this->keyStates[i] = false;
	}
}

bool Keyboard::KeyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}

bool Keyboard::OnKeyDown(const unsigned char keycode)
{
	if (KeyBufferIsEmpty()) // return empty KeyboardEvent if there are no keys to read
	{
		return false;
	}
	else // look up the keycode and return boolean if it is pressed or not
	{
		bool pressed = false;
		if (this->keyBuffer.count(keycode))
		{
			pressed = this->keyBuffer[keycode].IsPress();
			this->keyBuffer.erase(keycode);
		}
		return pressed;
	}
}

bool Keyboard::OnKeyUp(const unsigned char keycode)
{
	if (KeyBufferIsEmpty()) // return empty KeyboardEvent if there are no keys to read
	{
		return false;
	}
	else // look up the keycode and return boolean if it is released or not
	{
		bool released = false;
		if (this->keyBuffer.count(keycode))
		{
			released = this->keyBuffer[keycode].IsRelease();
			this->keyBuffer.erase(keycode);
		}
		return released;
	}
}

bool Keyboard::KeyBufferIsEmpty()
{
	return this->keyBuffer.empty();
}

void Keyboard::RegisterKeyPress(const unsigned char key)
{
	this->keyStates[key] = true;
	this->keyBuffer.insert({key, KeyboardEvent(KeyboardEvent::EventType::Press, key)});
}

void Keyboard::RegisterKeyRelease(const unsigned char key)
{
	this->keyStates[key] = false;
	this->keyBuffer.insert({key, KeyboardEvent(KeyboardEvent::EventType::Release, key)});
}

void Keyboard::EnableAutoRepeatKeys()
{
	this->autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys()
{
	this->autoRepeatKeys = false;
}

bool Keyboard::IsKeysAutoRepeat()
{
	return this->autoRepeatKeys;
}
