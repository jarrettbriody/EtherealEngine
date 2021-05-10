#pragma once
#include "MouseEvent.h"
#include <queue>

// TODO: Not confirmed that this set-up will work for input polling across multiple scripts

class Mouse
{
public:
	static bool SetupInstance();
	static Mouse* GetInstance();
	static bool DestroyInstance();
	
	void RegisterLMBPress(int x, int y);
	void RegisterLMBRelease(int x, int y);
	void RegisterRMBPress(int x, int y);
	void RegisterRMBRelease(int x, int y);
	void RegisterMMBPress(int x, int y);
	void RegisterMMBRelease(int x, int y);
	void RegisterWheelUp(int x, int y);
	void RegisterWheelDown(int x, int y);
	void RegisterMouseMove(int x, int y);

	bool LMBIsPressed();
	bool MMBIsPressed();
	bool RMBIsPressed();
	bool OnLMBDown();
	bool OnLMBUp();
	bool OnRMBDown();
	bool OnRMBUp();
	bool OnMMBDown();
	bool OnMMBUp();

	// TODO: Methods for mouse move, mouse wheel?

	bool EventBufferIsEmpty();
	int GetPosX();
	int GetPosY();
	MousePoint GetPos();

	void PurgeBuffers();

private:
	static Mouse* instance;

	bool leftIsDown = false;
	bool rightIsDown = false;
	bool mButtonIsDown = false;
	bool mouseInWindow = false;
	int x = 0;
	int y = 0;
	std::unordered_map<MouseEvent::EventType, MouseEvent> eventBuffer;
	
	bool OnMouseButtonDown(MouseEvent::EventType mouseEventType);
	bool OnMouseButtonUp(MouseEvent::EventType mouseEventType);
};

