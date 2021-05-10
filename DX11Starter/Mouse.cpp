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
	this->eventBuffer.insert({ MouseEvent::EventType::LPress, MouseEvent(MouseEvent::EventType::LPress, x, y) });
}

void Mouse::RegisterLMBRelease(int x, int y)
{
	this->leftIsDown = false;
	this->eventBuffer.insert({ MouseEvent::EventType::LRelease, MouseEvent(MouseEvent::EventType::LRelease, x, y) });
}

void Mouse::RegisterRMBPress(int x, int y)
{
	this->rightIsDown = true;
	this->eventBuffer.insert({ MouseEvent::EventType::RPress, MouseEvent(MouseEvent::EventType::RRelease, x, y) });
}

void Mouse::RegisterRMBRelease(int x, int y)
{
	this->rightIsDown = false;
	this->eventBuffer.insert({ MouseEvent::EventType::RRelease, MouseEvent(MouseEvent::EventType::RRelease, x, y) });
}

void Mouse::RegisterMMBPress(int x, int y)
{
	this->mButtonIsDown = true;
	this->eventBuffer.insert({ MouseEvent::EventType::MPress, MouseEvent(MouseEvent::EventType::MPress, x, y) });
}

void Mouse::RegisterMMBRelease(int x, int y)
{
	this->mButtonIsDown = false;
	this->eventBuffer.insert({ MouseEvent::EventType::MRelease, MouseEvent(MouseEvent::EventType::MRelease, x, y) });
}

void Mouse::RegisterWheelUp(int x, int y)
{
	this->eventBuffer.insert({ MouseEvent::EventType::WheelUp, MouseEvent(MouseEvent::EventType::WheelUp, x, y) });
}

void Mouse::RegisterWheelDown(int x, int y)
{
	this->eventBuffer.insert({ MouseEvent::EventType::WheelDown, MouseEvent(MouseEvent::EventType::WheelDown, x, y) });
}

void Mouse::RegisterMouseMove(int x, int y)
{
	this->x = x;
	this->y = y;

	this->eventBuffer.insert({ MouseEvent::EventType::Move, MouseEvent(MouseEvent::EventType::Move, x, y) }); // TODO: How useful is this/is it needed?
}

bool Mouse::OnMouseButtonDown(MouseEvent::EventType mouseEventType)
{
	if (EventBufferIsEmpty())
	{
		return false;
	}
	else
	{
		bool pressed = false;
		if (this->eventBuffer.count(mouseEventType))
		{
			pressed = true;
			this->eventBuffer.erase(mouseEventType);
		}
		return pressed;
	}
}

bool Mouse::OnMouseButtonUp(MouseEvent::EventType mouseEventType)
{
	if (EventBufferIsEmpty())
	{
		return false;
	}
	else
	{
		bool released = false;
		if (this->eventBuffer.count(mouseEventType))
		{
			released = true;
			this->eventBuffer.erase(mouseEventType);
		}
		return released;
	}
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
	return OnMouseButtonDown(MouseEvent::EventType::LPress);
}

bool Mouse::OnLMBUp()
{
	return OnMouseButtonUp(MouseEvent::EventType::LRelease);
}

bool Mouse::OnRMBDown()
{
	return OnMouseButtonDown(MouseEvent::EventType::RPress);
}

bool Mouse::OnRMBUp()
{
	return OnMouseButtonUp(MouseEvent::EventType::RRelease);
}

bool Mouse::OnMMBDown()
{
	return OnMouseButtonDown(MouseEvent::EventType::MPress);
}

bool Mouse::OnMMBUp()
{
	return OnMouseButtonDown(MouseEvent::EventType::MRelease);
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

void Mouse::PurgeBuffers()
{
	eventBuffer.clear();
}
