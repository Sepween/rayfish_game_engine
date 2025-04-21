#pragma once

#include <string>

#include "SDL2_mixer/SDL_mixer.h"

#include "EngineUtils.h"
#include "AudioHelper.h"
#include "Actor.h"

using namespace std;

class AudioDB {
public:
	AudioDB() {
		AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	}

	void init();

	void loadAudio(const string& audioName);
	Mix_Chunk* getAudio(const string& audioName);

	static int Play(int channel, const string& clipName, bool doesLoop);
	static void Halt(int channel);
	static void SetVolume(int channel, float volume);

	static AudioDB& getInstance() {
		static AudioDB instance;
		return instance;
	}

private:
	unordered_map<string, Mix_Chunk*> audioClips;
};