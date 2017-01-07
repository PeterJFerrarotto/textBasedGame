#ifndef _GAME_SOUND_H
#define _GAME_SOUND_H
#include "SDL_mixer.h"
#include "enumHelper.h"

enum COLLISION_STRENGTH { WEAK, MEDIUM, STRONG, COLLISION_STRENGTH_SIZE };
enum SOUND_TYPE { COLLISION_SOUND, AMBIANCE_SOUND, TRIGGER_SOUND, VOICE_SOUND, MUS_AMBIANCE, MUS_BACKGROUND, MUS_MENU };
enum SOUND_TRIGGER {TRIG_JUMP, TRIG_DESTROY, TRIG_DEACTIVATE, TRIG_ACTIVATE, TRIG_FIRE, TRIG_TIMER, TRIG_BOUNCE, TRIG_BOUNCE_HIGH, NUM_OF_TRIGS};

class GameSound
{
private:
	Mix_Chunk* sound;
	DIRECTION collisionDirection;
	COLLISION_STRENGTH collisionStrength;
protected:
	int loopCount;
	SOUND_TYPE soundType;
	SOUND_TRIGGER soundTrigger;
public:
	GameSound();
	~GameSound();

	SOUND_TYPE getSoundType();
	SOUND_TRIGGER getSoundTrigger();
	DIRECTION getCollisionDirection();
	COLLISION_STRENGTH getCollisionStrength();


	virtual void play(int channel = -1);
	virtual void loadFile(const char* fileLocation);

	void setSound(Mix_Chunk* sound);

	void setLoops(int loopCount);
	void setSoundType(SOUND_TYPE soundType);
	void setSoundTrigger(SOUND_TRIGGER soundTrigger);

	void setCollisionDirection(DIRECTION direction);
	void setCollisionStrength(COLLISION_STRENGTH strength);
};

#endif