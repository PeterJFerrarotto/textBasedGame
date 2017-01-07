#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "CompositeEntity.h"
#include "EnumClassHash.h"
#include "Tile.h"
#include "GameMusic.h"

#define PLAYER_TOP_SPEED 2.5
#define PLAYER_ACCELERATION 50
//60 fps : 1.0f/60.0f
#define FIXED_TIMESTEP 1/30.0f
#define MAX_TIMESTEP 6
#define BLINK_TIMING 0.25f
#define MAX_BLINK_COUNT 7
#define GRAVITY -100
#define STATIC_INDEX 10
#define FRAMES_PER_SECOND 6.0f

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GAME_STATE { TITLE_SCREEN, GAME_BEGIN, GAME_PLAY, GAME_END, GAME_QUIT, LEVEL_CHANGE };
enum SHAKE_DIREC {VERTICAL, HORIZONTAL};

class CollisionListener;
class CompositeEntity;
class ShaderProgram;
class Texture;
class Level;

static const float screenShakeSpeed = 8.0;

static const int maximumScreenShakes = 99;

class GameEngine final
{
private:
	std::vector<CollisionListener*> collisionEvents;
	std::unordered_map<std::string, std::unordered_map<unsigned, std::vector<CompositeEntity*>>> gameEntities;
	std::unordered_map<std::string, std::unordered_map<unsigned, std::vector<CompositeEntity*>>> gameEntitiesRenderingOrder;
	std::unordered_map<std::string, Level*> levels;
	CompositeEntity* playerEntity;
	std::vector<CompositeEntity*> userInterface;

	bool gameOver;

	int points;

	float blinkTime;
	float gameWall, gameCeiling;

	unsigned titleSelection;
	unsigned gameState;
	unsigned lives;
	unsigned enemyCount;
	unsigned blinkCount;
	unsigned mapWidth, mapHeight;

	Matrix viewMatrix;
	Matrix uIMatrix;

	std::string currentLevel;

	std::vector<GLfloat> backGroundVertices;

	//Checks collisions between dynamic entities
	void collisionCheck(float elapsed);
	//Checks collisions between dynamic entities and static entities
	void checkTileCollisions();
	void checkTileCollisionsSAT();
	void deleteFlagged();
	void handleInput(const Uint8* input, SDL_Event input2);
	bool downPressed;

	void render(ShaderProgram* program, float elapsed, float fps);

	void moveAll(unsigned entityType, float x, float y, float z = 0);
	void turnAll(unsigned entityType, bool turnX, bool turnY, bool turnZ = false);

	void changeState(unsigned newState);

	void changeTitleSelection();

	void update(float elapsed, ShaderProgram* program);

	float getTilePenetrationLeft(float posX, float sizeX, int tileX, float hitX, float hitWidth);
	float getTilePenetrationRight(float posX, float sizeX, int tileX, float hitX, float hitWidth);
	float getTilePenetrationUp(float posY, float sizeY, int tileY, float hitY, float hitHeight);
	float getTilePenetrationDown(float posY, float sizeY, int tileY, float hitY, float hitHeight);

	bool tileCollisionUp(float posY, float sizeYPos, float sizeYNeg, int tileY, float hitY, float hitHeight);
	bool tileCollisionDown(float posY, float sizeYNeg, float sizeYPos, int tileY, float hitY, float hitHeight);
	bool tileCollisionLeft(float posX, float sizeXNeg, float sizeXPos, int tileX, float hitX, float hitWidth);
	bool tileCollisionRight(float posX, float sizeXPos, float sizeXNeg, int tileX, float hitX, float hitWidth);

	bool shakeScreen;

	float checkBottomTileCollisions(CompositeEntity* entity);
	float checkTopTileCollisions(CompositeEntity* entity);
	float checkLeftTileCollisions(CompositeEntity* entity);
	float checkRightTileCollisions(CompositeEntity* entity);

	void resolveYCollisions(CompositeEntity* entity);
	void resolveXCollisions(CompositeEntity* entity);

	void resolveXYCollision(CompositeEntity* entity);

	Vector3 checkAllTileCollisions(CompositeEntity* entity);

	void checkIfShouldWarp();
	void deActivateLevelEntities();
	void activateLevelEntities();

	float screenShakeValue, screenShakeIntensity;

	int collisionMagnitude;

	unsigned int shakeCount;

	Vector3 tileVelocity;

	GameMusic* music;

	SHAKE_DIREC shakeDirection;
public:
	GameEngine();
	~GameEngine();

	void addCollisionEvent(CollisionListener* collisionEvent);
	void addGameEntity(const std::string& level, CompositeEntity* entity);
	void run(float elapsed, const Uint8* input, SDL_Event input2, ShaderProgram* program);
	void setLevel(const std::string& levelID);
	void addLevel(Level* level);

	void start();

	bool getGameOver();
	unsigned getPoints();

	void freeMemory();

};

#endif