#ifndef COMPOSITE_ENTITY_H
#define COMPOSITE_ENTITY_H
#include <string>
#include "Vector3.h"
#include "Entity.h"
#include "ParticleEmitter.h"
#include "GameSound.h"

enum ENTITY_TYPE { ICON_ENTITY, LIFE_ICON_ENTITY, PLAYER_PROJECTILE, ENEMY_PROJECTILE, TITLE_TEXT_ENTITY, GAME_TEXT_ENTITY, POINTS_INDICATOR, ACTOR_ENEMY, ACTOR_PLAYER, ACTOR_ENEMY_PATROL_TURN, STATIC_ENTITY, BACKGROUND_ENTITY, ENTITY_COIN, WARP_ENTITY, STATIC_FALL_ON_COLLDE, HIDING_ENEMY_FIRE, ENEMY_TURRET,ENTITY_TYPE_SIZE };
enum STATE {MOVING, ACCELERATING, STATIONARY, BOUNCING, BOUNCED, IDLE, JUMPING, FALLING, DESTROYING, DEACTIVATING, HIDING, STATE_COUNT};
enum COLLISION_BEHAVIOR { BOUNCE, STOP, DESTROY, DEACTIVATE, NOTHING, BOUNCE_HIGH, WARP, STATIC_COLLISION, FALL, COLLISION_BEHAVIOR_SIZE };
enum BOUNDARY_BEHAVIOR {BOUND_BOUNCE, BOUND_TURN, BOUND_STOP, BOUND_DESTROY, BOUND_DEACTIVATE, BOUND_NOTHING, BOUND_STOP_X, BOUND_DESTROY_NO_ANIM, BOUND_DEACTIVATE_UNDER_FLOOR};
enum TILE_LOGIC_BEHAVIOR {TURN_AT_EDGE, STOP_AT_EDGE, NO_LOGIC, LOGIC_SIZE};
enum TILE_COLLISION_BEHAVIOR {T_STOP, T_BOUNCE, T_TURN, T_NOTHING};

class ShaderProgram;
class Texture;
class CompositeEntity
{
protected:
	Entity* first;

	Entity* collisionNode;
	Entity* otherCollisionNode;

	std::unordered_map<unsigned, std::vector<ParticleEmitter*>> particleEmitters;
	//Stores collision sounds based upon direction and collision strength
	std::unordered_map<unsigned, std::unordered_map<unsigned,GameSound*>> collisionSounds;
	//Stores triggered sounds based upon trigger
	std::unordered_map<unsigned, GameSound*> triggerSounds;

	Vector3 position;
	Vector3 startingPosition;
	Vector3 velocity;
	Vector3 startingVelocity;
	Vector3 acceleration;
	Vector3 startingAcceleration;
	Vector3 checkPointPosition;
	Vector3 sizePositive;
	Vector3 sizeNegative;
	Vector3 scale;
	Vector3 startingScale;

	Matrix modelMatrix;

	unsigned renderLayer;
	unsigned objectLayer;
	CompositeEntity* projectile;

	GLuint textSheet;
	float rotation;
	float startingRotation;
	float rotationalVelocity;
	float startingRotationalVelocity;
	float topSpeed;
	float firingDelay;
	float jumpSpeed;
	//For TEXT_ENTITY only:
	float spacing, size;

	int awarenessRadius, hidingRadius;

	ENTITY_TYPE type;
	BOUNDING_TYPE boundingType;
	COLLISION_BEHAVIOR collisionBehavior;
	BOUNDARY_BEHAVIOR boundaryBehavior;
	STATE state;
	TILE_COLLISION_BEHAVIOR tileCollisionBehavior;
	TILE_LOGIC_BEHAVIOR tileLogicBehavior;

	bool isActive;
	bool doDelete;
	bool collideLeft, collideRight, collideTop, collideBottom;
	bool staticCollideLeft, staticCollideRight, staticCollideTop, staticCollideBottom;
	COLLISION_STRENGTH collisionStrength;
	bool boundaryLeft, boundaryRight, boundaryTop, boundaryBottom;
	bool canCollide;
	bool canCollideWithTiles;
	bool falls;
	bool subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat, CompositeEntity* that);
	bool subEntitiesCollidingSAT(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat, CompositeEntity* that);
	bool isInvincible;
	bool onTileGround;
	bool doMirror;
	bool overrideMirroring;
	bool isStatic;
	bool firing;


	void bounce(float elapsed, CompositeEntity* bouncingOffOf);
	void bounceHigh(float elapsed, CompositeEntity* bouncingOffOf);
	void stop();
	void fall();
	void resetFlags();
	void transformMatrix();

	void postCollisionAction(GameSound* sound = nullptr);



	std::string entityID;
	std::string text;
	std::string warpLevelDestination;

	void boundaryStop(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling);
	void boundaryStopAtWall(float gameWallLeft, float gameWallRight);
	void boundaryTurn(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling);

	void centralize();

	void projectToAxis();

	bool centralized;

	std::vector<Vector3> SATcoordinates;

	unsigned bottomStaticCollisionCount;
public:
	CompositeEntity();
	CompositeEntity(Entity* first);
	CompositeEntity(Entity* first, float firingDelay);
	CompositeEntity(CompositeEntity* toCopy);
	~CompositeEntity();

	Entity* getEntities();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	//Vector3 getTotalSize();
	Vector3 getSizePositive();
	Vector3 getSizeNegative();
	Vector3 getScale();
	float getRotation();
	float getRotationalVelocity();
	float getTopSpeed();
	float timeSinceFiring;
	float getJumpSpeed();
	BOUNDING_TYPE getBoundingType();
	ENTITY_TYPE getEntityType();
	COLLISION_BEHAVIOR getCollisionBehavior();
	BOUNDARY_BEHAVIOR getBoundaryBehavior();
	TILE_COLLISION_BEHAVIOR getTileCollisionBehavior();
	TILE_LOGIC_BEHAVIOR getTileLogicBehavior();
	STATE getState();
	bool getIsActive();
	bool getDoDelete();
	bool getCanCollide();
	bool getisInvincible();
	bool getOnGround();
	bool getAtBottom();
	bool getFalls();
	bool getCollUpFlag();
	bool getCollDownFlag();
	bool getStaticCollDownFlag();
	bool getCollLeftFlag();
	bool getCollRightFlag();
	bool getIsStatic();
	bool getCanCollideWithTiles();
	unsigned getObjectLayer();
	unsigned getRenderLayer();
	const std::string& getWarpDestination();

	const std::string& getEntityID();




	void setEntities(Entity* first);
	void setPosition(float x, float y, float z = 0.0);
	void setStartingPosition(float x, float y, float z = 0.0);
	void setVelocity(float x, float y, float z = 0.0);
	void setStartingVelocity(float x, float y, float z = 0.0);
	void setAcceleration(float x, float y, float z = 0.0);
	void setStartingAcceleration(float x, float y, float z = 0.0);
	void setScale(float x, float y, float z = 1.0);
	void setStartingScale(float x, float y, float z = 1.0);
	void setFiringDelay(float firingDelay);
	void setRotation(float rotation);
	void setStartingRotation(float startingRotation);
	void setRotationalVelocity(float rotation);
	void setStartingRotationalVelocity(float startingRotationalVelocity);
	void setBoundingType(BOUNDING_TYPE boundingType);
	void setEntityType(ENTITY_TYPE type);
	void setCollisionBehavior(COLLISION_BEHAVIOR behavior);
	void setBoundaryBehavior(BOUNDARY_BEHAVIOR behavior);
	void setTileCollisionBehavior(TILE_COLLISION_BEHAVIOR behavior);
	void setTileLogicBehavior(TILE_LOGIC_BEHAVIOR behavior);
	void setState(STATE state);
	void setIsActive(bool isActive, bool playSound = false);
	void setCanCollide(bool canCollide);
	void setisInvincible(bool isInvincible);
	void setFalls(bool falls);
	void setEntityID(const std::string& entityID);
	void setSpacing(float spacing);
	void setSize(float size);
	void setTopSpeed(float topSpeed);
	void setRenderLayer(unsigned renderLayer);
	void setObjectLayer(unsigned objectLayer);
	void setOnTileGround(bool onTileGround);
	void setIsStatic(bool isStatic);
	void setWarpDestination(const std::string& level);
	void checkPoint();
	void setAwarenessRadius(int radius);
	void setHidingRadius(int radius);
	void setCanCollideWithTiles(bool canCollide);

	void jump();
	void updateBounding();
	void updateBoundingRecurse(Entity* check, Vector3 offsetScale, float offsetX = 0, float offsetY = 0, float offsetZ = 0);
	void reset();
	void resetToCheckpoint();
	bool isColliding(CompositeEntity* collidingCheck);
	bool isCollidingSAT(CompositeEntity* collidingCheck, DIRECTION collisionDirection);
	bool atScreenBoundary(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling);
	void move(float elapsed, float gravity = 0.0f, float frictionX = 0.0f, float frictionY = 0.0f);
	void draw(ShaderProgram* program, Matrix matrix, float elapsed, float fps);
	void drawText(ShaderProgram* program, Matrix matrix, float elapsed, float fps);
	void collide(float elapsed, CompositeEntity* collidingWith, COLLISION_BEHAVIOR collisionBehavior = COLLISION_BEHAVIOR_SIZE, GameSound* sound = nullptr);

	void collideSAT(float elapsed, CompositeEntity* collidingWith, COLLISION_BEHAVIOR collisionBehavior = COLLISION_BEHAVIOR_SIZE, GameSound* sound = nullptr);

	//void collideWithStatic(float penetration, DIRECTION direction);
	void collideWithStatic(CompositeEntity* collidingWith);
	void collideWithStaticSAT(CompositeEntity* collidingWith);
	void boundaryAction(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling);
	void setJumpSpeed(float jumpSpeed);
	void setDoMirror(bool doMirror);
	void setOverrideMirroring(bool overrideMirroring);
	void blink();
	void addParticleEmitter(ParticleEmitter* emitter);

	void setAllRender(bool doRender);

	void changeAnimation(ANIMATION_TYPE animationType);
	void runAnimation(float elapsed, float fps);

	void hardCollision(DIRECTION collisionDirection, int particleCount = 3);

	CompositeEntity* fire();
	void setProjectile(CompositeEntity* projectile);

	void addToTimeSinceFiring(float elapsed);

	 CompositeEntity* logic(CompositeEntity* player, CompositeEntity* lastProjectile = nullptr);

	void setDisplayText(const std::string& text);

	const std::string& getDisplayText();

	void setTextSheet(GLuint textSheet);

	void deepCopy(CompositeEntity* toCopy);

	void destroy(bool doAnimate, bool playSound);
	void deActivate();

	void addCollisionSound(GameSound* gameSound);
	void addTriggerSound(GameSound* sound);

	void playCollisionSound(DIRECTION direction = DIRECTION_SIZE, COLLISION_STRENGTH collisionStrength = COLLISION_STRENGTH_SIZE);
	void playTriggerSound(SOUND_TRIGGER trigger);


	Matrix& getMatrix();

	void freeMemory();

	const std::vector<Vector3>& getSATCoordinates();
};

#endif