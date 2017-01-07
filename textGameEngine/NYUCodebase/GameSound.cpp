#include "GameSound.h"
#include <unordered_map>

GameSound::GameSound()
{
}


GameSound::~GameSound()
{
}

SOUND_TYPE GameSound::getSoundType(){
	return soundType;
}

SOUND_TRIGGER GameSound::getSoundTrigger(){
	return soundTrigger;
}

DIRECTION GameSound::getCollisionDirection(){
	return collisionDirection;
}

COLLISION_STRENGTH GameSound::getCollisionStrength(){
	return collisionStrength;
}

void GameSound::play(int channel){
	Mix_PlayChannel(channel, sound, loopCount);
}

void GameSound::loadFile(const char* fileLocation){
	static std::unordered_map<std::string, Mix_Chunk*> loadedSounds;
	if (loadedSounds.size() == 0 || loadedSounds.find(fileLocation) == loadedSounds.end()){
		sound = Mix_LoadWAV(fileLocation);
		loadedSounds[fileLocation] = sound;
	}
	else{
		sound = loadedSounds[fileLocation];
	}
}

void GameSound::setSound(Mix_Chunk* sound){
	this->sound = sound;
}

void GameSound::setLoops(int loopCount){
	this->loopCount = loopCount;
}

void GameSound::setSoundType(SOUND_TYPE soundType){
	this->soundType = soundType;
}

void GameSound::setSoundTrigger(SOUND_TRIGGER soundTrigger){
	this->soundTrigger = soundTrigger;
}

void GameSound::setCollisionDirection(DIRECTION direction){
	collisionDirection = direction;
}

void GameSound::setCollisionStrength(COLLISION_STRENGTH strength){
	collisionStrength = strength;
}