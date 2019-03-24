#include <stddef.h>
#include "Soundplayer.h"

SoundPlayer::SoundPlayer() {
	FMOD::System_Create(&system);

	system->init(32, FMOD_INIT_NORMAL, nullptr);
}

SoundPlayer::~SoundPlayer() {
	system->close();
	system->release();
}


FMOD::Sound* SoundPlayer::createSound(const char* pFile)
{
	FMOD::Sound* sound;
	system->createSound(pFile, FMOD_DEFAULT, nullptr, &sound);

	return sound;
}

void SoundPlayer::playSound(sound_t* pSound, bool bLoop, int loop_times) {
	FMOD::Sound* sound = pSound->sound;

	if (!bLoop)
		sound->setMode(FMOD_LOOP_OFF);
	else {
		sound->setMode(FMOD_LOOP_NORMAL);
		sound->setLoopCount(loop_times);
	}

	system->playSound(sound, nullptr, false, &(pSound->channel));
}

void SoundPlayer::releaseSound(sound_t* pSound, bool stop) {
	if (stop && pSound->channel)
		pSound->channel->stop();

	pSound->sound->release();
}

void SoundPlayer::update()
{
	system->update();
}

void SoundPlayer::changeVolume(sound_t* pSound, float volume) {
	if (pSound->channel)
		pSound->channel->setVolume(volume);
}