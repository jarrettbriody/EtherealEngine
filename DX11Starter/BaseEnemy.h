#pragma once

class BaseEnemy
{
public:
	virtual void IsLeashed(bool leashed, float delay) = 0;
	virtual void CheckPlayerState() = 0;
};

