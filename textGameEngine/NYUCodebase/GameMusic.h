#ifndef _GAME_MUSIC_H
#define _GAME_MUSIC_H
#include "GameSound.h"

class GameMusic final : public GameSound
{
private:
	Mix_Music* music;
public:
	GameMusic();
	~GameMusic();

	//For music, does not need a channel to play on.
	virtual void play(int channel = 0);
	void stop();
	virtual void loadFile(const char* fileLocation);

	void setMusic(Mix_Music* music);
};

#endif