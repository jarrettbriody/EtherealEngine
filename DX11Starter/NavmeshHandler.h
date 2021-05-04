#pragma once
class NavmeshHandler
{
private:
	static NavmeshHandler instance;
public:
	static bool SetupInstance();
	static NavmeshHandler* GetInstance();
	static bool DestroyInstance();
};

