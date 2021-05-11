#pragma once
#include "pch.h"
#include "Config.h"

class AudioManager
{
public:
	static FMOD_RESULT F_CALLBACK ChannelCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2);
	static void FMODErrorCheck(FMOD_RESULT result);
	static void StartCombatTheme();
};