#pragma once

#include <fmod.hpp>
#include <fmod_common.h>

struct sound_t {
	sound_t(FMOD::Sound* sound) {
		this->sound = sound;
	}

	FMOD::Sound* sound;
	FMOD::Channel* channel;
};

class SoundPlayer {
public:
	SoundPlayer();
	~SoundPlayer();

	static SoundPlayer &instance()
	{
		static SoundPlayer S;

		return S;
	}

	FMOD::Sound* createSound(const char* pFile);

	void playSound(sound_t* pSound, bool bLoop = false, int loop_times = -1);

	void releaseSound(sound_t* pSound, bool stop = false);

	void update();

	void changeVolume(sound_t* pSound, float volume);

private:
	FMOD::System* system;
};
