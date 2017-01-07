#include "GameMusic.h"
#include <unordered_map>

GameMusic::GameMusic()
{
}


GameMusic::~GameMusic()
{
}

void GameMusic::play(int channel){
	Mix_PlayMusic(music, loopCount);
}

void GameMusic::stop(){
	Mix_PauseMusic();
}

void GameMusic::loadFile(const char* fileLocation){
	static std::unordered_map<std::string, Mix_Music*> loadedMusic;
	if (loadedMusic.size() == 0 || loadedMusic.find(fileLocation) == loadedMusic.end()){
		music = Mix_LoadMUS(fileLocation);
		loadedMusic[fileLocation] = music;
	}
	else{
		music = loadedMusic[fileLocation];
	}
}

void GameMusic::setMusic(Mix_Music* music){
	this->music = music;
}