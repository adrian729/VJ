#pragma once

#include <fmod.hpp>
#include <fmod_common.h>
#include <string>

struct audio {
	audio() {};
	audio(FMOD::Sound *sound) {
		this->sound = sound;
	}
	FMOD::Sound *sound;
	FMOD::Channel *channel;
};

class Audio {
public:
	Audio();
	~Audio();

	static Audio &instance() {
		static Audio a;
		return a;
	}

	// if times = -1, infinite loop
	FMOD::Sound *createAudio(const char *file);
	void play(audio *sound, int times = -1, float volume = 1.f);
	void release(audio *sound);
	void update();
	void setVolume(audio *sound, float volume);

private:
	FMOD::System* sys;
};