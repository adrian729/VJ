#include "Audio.h"
#include <iostream>

using namespace std;

Audio::Audio() {
	FMOD::System_Create(&sys);
	// maxchannels, fmod initflag, extradriver
	sys->init(32, FMOD_INIT_NORMAL, nullptr);
	//sys->init(36, FMOD_INIT_CHANNEL_DISTANCEFILTER, nullptr);
}

Audio::~Audio() {
	sys->close();
	sys->release();
}

FMOD::Sound *Audio::createAudio(const char *file) {
	FMOD::Sound *sound;
	// fileName, mode, extended info, sound
	sys->createSound(file, FMOD_DEFAULT, nullptr, &sound);

	return sound;
}

void Audio::play(audio *sound, int times, float volume) {
	// times == -1: infinite loop
	if (times != -1) times = times - 1;
	FMOD::Sound *soundPlay = sound->sound;
	int mode = FMOD_LOOP_NORMAL;
	if (times == 0) mode = FMOD_LOOP_OFF;
	else soundPlay->setLoopCount(times);
	soundPlay->setMode(mode);

	sys->playSound(soundPlay, nullptr, false, &(sound->channel));
	if (sound->channel) sound->channel->setVolume(volume);
}

void Audio::release(audio *sound) {
	if (sound->channel) sound->channel->stop();
	sound->sound->release();
}

void Audio::update() {
	sys->update();
}

void Audio::setVolume(audio *sound, float volume) {
	if (sound->channel) sound->channel->setVolume(volume);
}
