#include "pch.h"
#include "AudioManager.h"

FMOD_RESULT F_CALLBACK AudioManager::ChannelCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void* commandData1, void* commandData2)
{
	//std::cout << "Callback called for " << controlType << std::endl;

	if (controlType == FMOD_CHANNELCONTROL_CHANNEL && callbackType == FMOD_CHANNELCONTROL_CALLBACK_END) // Also needs to be able to check the scene we are in
	{
		//std::cout << "I finished playing!" << std::endl;
		FMOD::Channel* channel = (FMOD::Channel*)channelControl;
		FMOD::Sound* sound;
		channel->getCurrentSound(&sound);

		if (sound == Config::MainTheme[0] || sound == Config::MainTheme[1] || sound == Config::MainTheme[2])
		{
			//std::cout << "We are probably starting the game!" << std::endl;
			StartCombatTheme();
		}
	}

	return FMOD_OK;
}

void AudioManager::FMODErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}
}

void AudioManager::StartCombatTheme()
{
	/*
	// Variables for sound scheduling
	unsigned int dsp_block_len, count;
	int outputrate = 0;

	// Get information needed later for scheduling.  The mixer block size, and the output rate of the mixer.
	Config::FMODResult = Config::FMODSystem->getDSPBufferSize(&dsp_block_len, 0);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->getSoftwareFormat(&outputrate, 0, 0);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	// Play the combat theme
	for (count = 0; count < 2; count++)
	{
		static unsigned long long clock_start = 0;
		unsigned int slen;

		Config::FMODResult = Config::FMODSystem->playSound(Config::CombatTheme[count], Config::MusicGroup, true, &Config::MusicChannel); // Start a part of the combat theme, but leave it paused
		AudioManager::FMODErrorCheck(Config::FMODResult);

		if (!clock_start)
		{
			Config::FMODResult = Config::MusicChannel->getDSPClock(0, &clock_start);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			clock_start += (dsp_block_len * 2); // Set the first delay to something small, just so we can finish setting up the timing
		}
		else
		{
			float freq;
			FMOD::Sound* previousSound = Config::CombatTheme[count - 1];

			Config::FMODResult = previousSound->getLength(&slen, FMOD_TIMEUNIT_PCM);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			Config::FMODResult = previousSound->getDefaults(&freq, 0);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			slen = (unsigned int)((float)slen / freq * outputrate); // Calculate the length of the previously played sound

			clock_start += slen; // Set the new delay to after the previous sound finishes
		}

		Config::FMODResult = Config::MusicChannel->setDelay(clock_start, 0, false); // Set a delay for the currently playing sound
		AudioManager::FMODErrorCheck(Config::FMODResult);

		Config::FMODResult = Config::MusicChannel->setPaused(false); // Unpause this section of the combat theme so it will play after the delay passes
		AudioManager::FMODErrorCheck(Config::FMODResult);
	}*/
}