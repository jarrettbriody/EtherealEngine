#pragma once
#include "MouseEvent.h"
#include <queue>

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

	bool EventBufferIsEmpty();
	int GetPosX();
	int GetPosY();
	MousePoint GetPos();
	MouseEvent ReadEvent();

private:
	static Mouse* instance;

	bool leftIsDown = false;
	bool rightIsDown = false;
	bool mButtonIsDown = false;
	int x = 0;
	int y = 0;
	std::queue<MouseEvent> eventBuffer;
};

