#include "AudioDB.h"

void AudioDB::init() {
	AudioHelper::Mix_AllocateChannels(50);
}

void AudioDB::loadAudio(const string& audioName) {
	if (audioClips.find(audioName) != audioClips.end()) {
		return;
	}

	string audioPath = EngineUtils::getAudioWav(audioName, ".wav");
	Mix_Chunk* audio = AudioHelper::Mix_LoadWAV(audioPath.c_str());
	if (!audio) {
		audioPath = EngineUtils::getAudioWav(audioName, ".ogg");
		audio = AudioHelper::Mix_LoadWAV(audioPath.c_str());
	}
	if (!audio) {
		cout << "error: failed to play audio clip " << audioName;
		exit(0);
	}
	audioClips[audioName] = audio;
}

Mix_Chunk* AudioDB::getAudio(const string& audioName) {
	loadAudio(audioName);
	return audioClips[audioName];
}

int AudioDB::Play(int channel, const string& audioName, bool doesLoop) {
	Mix_Chunk* audio = getInstance().getAudio(audioName);
	int loops = doesLoop ? -1 : 0;

	return AudioHelper::Mix_PlayChannel(channel, audio, loops);
}

void AudioDB::Halt(int channel) {
	AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(int channel, float volume) {
	int volumeInt = static_cast<int>(volume);
	volumeInt = max(0, volumeInt);
	volumeInt = min(128, volumeInt);

	AudioHelper::Mix_Volume(channel, volumeInt);
}
