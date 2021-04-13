#include "pch.h"
#include "Mouse.h"

Mouse* Mouse::instance = nullptr;

bool Mouse::SetupInstance()
{
	if (instance == nullptr)
	{
		instance = new Mouse();
		return true;
	}
	return false;
}

Mouse* Mouse::GetInstance()
{
	return instance;
}

bool Mouse::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
}

void Mouse::RegisterLMBPress(int x, int y)
{
	this->leftIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
}

void Mouse::RegisterLMBRelease(int x, int y)
{
	this->leftIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
}

void Mouse::RegisterRMBPress(int x, int y)
{
	this->rightIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void Mouse::RegisterRMBRelease(int x, int y)
{
	this->rightIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void Mouse::RegisterMMBPress(int x, int y)
{
	this->mButtonIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void Mouse::RegisterMMBRelease(int x, int y)
{
	this->mButtonIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void Mouse::RegisterWheelUp(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void Mouse::RegisterWheelDown(int x, int y)
{
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void Mouse::RegisterMouseMove(int x, int y)
{
	this->x = x;
	this->y = y;
	this->eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

bool Mouse::LMBIsPressed()
{
	return this->leftIsDown;
}

bool Mouse::MMBIsPressed()
{
	return this->mButtonIsDown;
}

bool Mouse::RMBIsPressed()
{
	return this->rightIsDown;
}

bool Mouse::OnLMBDown()
{
	return ReadEvent().GetType() == MouseEvent::LPress;
}

bool Mouse::OnLMBUp()
{
	return ReadEvent().GetType() == MouseEvent::LRelease;
}

bool Mouse::OnRMBDown()
{
	return ReadEvent().GetType() == MouseEvent::RPress;
}

bool Mouse::OnRMBUp()
{
	return ReadEvent().GetType() == MouseEvent::RRelease;
}

bool Mouse::OnMMBDown()
{
	return ReadEvent().GetType() == MouseEvent::MPress;;
}

bool Mouse::OnMMBUp()
{
	return ReadEvent().GetType() == MouseEvent::MRelease;
}

bool Mouse::EventBufferIsEmpty()
{
	return this->eventBuffer.empty();
}

int Mouse::GetPosX()
{
	return this->x;
}

int Mouse::GetPosY()
{
	return this->y;
}

MousePoint Mouse::GetPos()
{
	return { this->x, this->y };
}

MouseEvent Mouse::ReadEvent()
{
	if (this->eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent me = this->eventBuffer.front();
		this->eventBuffer.pop();
		return me;
	}
}
