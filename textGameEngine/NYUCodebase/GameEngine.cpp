#include "GameEngine.h"
#include "CompositeEntity.h"
#include "CollisionListener.h"
#include "Texture.h"
#include "Entity.h"
#include "mathHelper.h"
#include "Level.h"
#include "Tile.h"
#include "ShaderProgram.h"
using namespace std;



GameEngine::GameEngine() : gameOver(false), titleSelection(0)
{
	enemyCount = 0;
	tileVelocity.x = 0;
	tileVelocity.y = 0;
	tileVelocity.z = 0;
}

GameEngine::~GameEngine()
{
}

void GameEngine::render(ShaderProgram* program, float elapsed, float fps){
	glClear(GL_COLOR_BUFFER_BIT);

	uIMatrix.identity();
	viewMatrix.identity();
	viewMatrix.setScale(0.02, 0.01, 1);
	if (playerEntity->getPosition().x >= 180 && playerEntity->getPosition().x <= mapWidth - 180){
		viewMatrix.Translate(-playerEntity->getPosition().x, 0, 0);
		uIMatrix.Translate(playerEntity->getPosition().x - 172, 0, 0);
	}
	else if (playerEntity->getPosition().x < 180){
		viewMatrix.Translate(-180, 0, 0);
		uIMatrix.Translate(8, 0, 0);
	}
	else if (playerEntity->getPosition().x > mapWidth - 180){
		viewMatrix.Translate(-(int)(mapWidth - 180), 0, 0);
		uIMatrix.Translate((int)(mapWidth - 160), 0, 0);
	}
	
	if (playerEntity->getPosition().y >= -(int)(mapHeight - 200) && playerEntity->getPosition().y <= -16){
		viewMatrix.Translate(0, -playerEntity->getPosition().y, 0);
		uIMatrix.Translate(0, playerEntity->getPosition().y + 180, 0);
	}
	else if (playerEntity->getPosition().y < -(int)(mapHeight - 200)){
		viewMatrix.Translate(0, mapHeight - 200, 0);
		uIMatrix.Translate(0, -(int)(mapHeight - 380), 0);
	}
	else if (playerEntity->getPosition().y > -16){
		viewMatrix.Translate(0, 16, 0);
		uIMatrix.Translate(0, -32, 0);
	}
	if (shakeScreen && shakeCount < maximumScreenShakes){
		screenShakeValue += elapsed;
		screenShakeIntensity = shakeDirection == VERTICAL ? 5 : 2.5;
		screenShakeIntensity *= collisionMagnitude;
		shakeCount++;
	}
	else{
		shakeCount = 0;
		shakeScreen = false;
		screenShakeValue += elapsed;
		screenShakeIntensity = easeOut(screenShakeIntensity, 0, elapsed);
		if (screenShakeIntensity <= 0.001){
			screenShakeIntensity = 0;
		}
	}
	
	if (shakeDirection == VERTICAL){
		viewMatrix.Translate(0.0f, sin(screenShakeValue * screenShakeSpeed) * screenShakeIntensity, 0.0f);
	}
	else if (shakeDirection == HORIZONTAL){
		viewMatrix.Translate(sin(screenShakeValue * screenShakeSpeed) * screenShakeIntensity, 0, 0.0f);
	}

	program->setViewMatrix(viewMatrix);
	Level* lvl = levels[currentLevel];
	int layerCount = getEntityLayers(gameEntitiesRenderingOrder, currentLevel) > lvl->getLayerCount() ? getEntityLayers(gameEntitiesRenderingOrder, currentLevel) : lvl->getLayerCount();
	for (size_t i = 0; i <= layerCount; i++){
		//glClearColor(0.03921568627, 0.59607843137, 0.67450980392, 1.0);
		glClearColor(lvl->getBackgroundColor().r, lvl->getBackgroundColor().g, lvl->getBackgroundColor().b, lvl->getBackgroundColor().a);
		levels[currentLevel]->draw(program, i);
		if (gameEntitiesRenderingOrder[currentLevel].find(i) != gameEntitiesRenderingOrder[currentLevel].end()){
			for (vector<CompositeEntity*>::const_iterator itr = gameEntitiesRenderingOrder[currentLevel][i].begin(); itr != gameEntitiesRenderingOrder[currentLevel][i].end(); itr++){
				if ((*itr)->getIsActive() && (*itr)->getEntityType() != POINTS_INDICATOR)
					(*itr)->draw(program, uIMatrix, elapsed, fps);
			}
		}
	}
	for (size_t i = 0; i < userInterface.size(); i++){
		userInterface[i]->draw(program, uIMatrix, elapsed, fps);
	}
}

void GameEngine::addGameEntity(const std::string& level, CompositeEntity* entity){
	if (entity->getEntityType() != BACKGROUND_ENTITY){
		gameEntities[level][entity->getEntityType()].push_back(entity);
		gameEntitiesRenderingOrder[level][entity->getRenderLayer()].push_back(entity);
		//if (entity->getEntityType() == ACTOR_PLAYER){
		//	playerEntity = entity;
		//}
		if (entity->getEntityType() == ACTOR_ENEMY){
			enemyCount++;
		}
		if (entity->getEntityType() == POINTS_INDICATOR){
			userInterface.push_back(entity);
		}
	}
}

void GameEngine::addCollisionEvent(CollisionListener* collisionEvent){
	collisionEvents.push_back(collisionEvent);
}

void GameEngine::collisionCheck(float elapsed){
	for (size_t i = 0; i < collisionEvents.size(); i++){
		(*collisionEvents[i])(elapsed, gameEntities[currentLevel]);		
	}
}

void GameEngine::setLevel(const std::string& levelID){
	//if (backGroundTextures.find(levelID) == backGroundTextures.end()){
	//	throw "Unknown levelID!";
	//}
	deActivateLevelEntities();
	this->currentLevel = levelID;
	activateLevelEntities();
	this->mapHeight = levels[levelID]->getMapHeight();
	this->mapWidth = levels[levelID]->getMapLength();
	this->playerEntity = levels[levelID]->getPlayerEntity();
}

void GameEngine::addLevel(Level* level){
	if (levels.find(level->getLevelID()) != levels.end()){
		throw "Level already exists!";
	}
	levels[level->getLevelID()] = level;
}

float GameEngine::getTilePenetrationDown(float posY, float sizeY, int tileY, float hitY, float hitHeight){
	return abs(posY - sizeY) - (levels[currentLevel]->getTileSize() * (tileY) + hitY) + 0.001;
}

float GameEngine::getTilePenetrationUp(float posY, float sizeY, int tileY, float hitY, float hitHeight){
	return ((posY + sizeY)) - (-(levels[currentLevel]->getTileSize() * (tileY) + (hitHeight - hitY))) + 0.001;
}

float GameEngine::getTilePenetrationLeft(float posX, float sizeX, int tileX, float hitX, float hitWidth){
	return ((levels[currentLevel]->getTileSize() * (tileX)) + (hitWidth - hitX)) - (posX - sizeX) + 0.001;
}

float GameEngine::getTilePenetrationRight(float posX, float sizeX, int tileX, float hitX, float hitWidth){
	return (posX + sizeX) - ((levels[currentLevel]->getTileSize() * tileX) + hitX) + 0.001;
}

//Tile collision up if top of object is higher than bottom of tile and bottom of tile is higher than bottom of object
bool GameEngine::tileCollisionUp(float posY, float sizeYPos, float sizeYNeg, int tileY, float hitY, float hitHeight){
	return ((posY + sizeYPos) >= (-((tileY) * levels[currentLevel]->getTileSize() + (hitHeight - hitY)))) && ((-((tileY) * levels[currentLevel]->getTileSize() + (hitHeight - hitY))) >= (posY - sizeYNeg));
}

//Tile collision down if top of tile is higher than bottom of object and top of object is higher than top of tile
bool GameEngine::tileCollisionDown(float posY, float sizeYNeg, float sizeYPos, int tileY, float hitY, float hitHeight){
	//return posY - sizeYNeg >= (-(tileY + 1) * levels[currentLevel]->getTileSize());
	return (-((tileY)* levels[currentLevel]->getTileSize() + hitY) >= posY - sizeYNeg) && (posY + sizeYPos >= -((tileY)*levels[currentLevel]->getTileSize() + hitY));
}

//Tile collision left if left of object is farther right than left of tile
bool GameEngine::tileCollisionLeft(float posX, float sizeXNeg, float sizeXPos, int tileX, float hitX, float hitWidth){
	return posX - sizeXNeg >= ((tileX) * levels[currentLevel]->getTileSize() + hitX);
}

//Tile collision right if right of tile is farther right than right of object
bool GameEngine::tileCollisionRight(float posX, float sizeXPos, float sizeXNeg, int tileX, float hitX, float hitWidth){
	//return ((tileX) * levels[currentLevel]->getTileSize()) >= posX + sizeX;
	return ((tileX) * levels[currentLevel]->getTileSize() + (hitWidth - hitX)) >= posX + sizeXPos;
}


float GameEngine::checkBottomTileCollisions(CompositeEntity* entity){
	if (entity->getIsActive()){
		float rC = cos(entity->getRotation());
		float rS = sin(entity->getRotation());
		float posX = entity->getPosition().x;
		float posY = entity->getPosition().y;
		float sizeXPos = abs(entity->getSizePositive().x * rC + entity->getSizePositive().y * rS);
		float sizeXNeg = abs(entity->getSizeNegative().x * rC - entity->getSizePositive().y * rS);
		float sizeY = abs(entity->getSizeNegative().y * rC - entity->getSizePositive().x * rS);
		float tileSize = levels[currentLevel]->getTileSize();
		int gridRight = (int)((posX + sizeXPos) / tileSize);
		int gridLeft = (int)((posX - sizeXNeg) / tileSize);
		int gridBottom = (int)(floor(abs((posY - sizeY) / tileSize)));
		int oneAboveGridBottom = gridBottom - 1;
		unsigned layer = entity->getObjectLayer();
		float penetration = 0;

		if (gridBottom < 0 || oneAboveGridBottom < 0){
			entity->boundaryAction(0, mapWidth, -(int)mapHeight, 0);
		}

		Level* tmp = levels[currentLevel];
		Tile* tmpTile;
		Tile* tmpTile2;
		if (abs(posY - sizeY) < mapHeight){
			for (int i = gridLeft; i <= gridRight; i++){
				tmpTile = tmp->getTile(layer, i, gridBottom);
				tmpTile2 = tmp->getTile(layer, i, oneAboveGridBottom);
				if (tmpTile != nullptr){
					if ((tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == SOLID_PLATFORM || tmpTile->tileType == SOLID_GRASS) && tmpTile->canCollide && (tmpTile2 == nullptr || !((tmpTile2->tileType == SOLID_FLOOR || tmpTile2->tileType == SOLID_PLATFORM || tmpTile2->tileType == SOLID || tmpTile2->tileType == SOLID_GRASS) && tmpTile2->canCollide))){
						if (tileCollisionDown(posY, sizeY, abs(entity->getSizePositive().y * rC + entity->getSizePositive().x * rS), gridBottom, tmpTile->y, tmpTile->height) && !(tmpTile->tileType == SOLID_PLATFORM && downPressed)){
							if (!(posY - sizeY < -(gridBottom * tileSize + tileSize / 2))){
								if (getTilePenetrationDown(posY, sizeY, gridBottom, tmpTile->y, tmpTile->height) > penetration){
									penetration = getTilePenetrationDown(posY, sizeY, gridBottom, tmpTile->y, tmpTile->height);
									if (entity->getTileLogicBehavior() == TURN_AT_EDGE){
										if (gridLeft * tileSize - (posX - sizeXNeg) <= 0.0001){
											tmpTile = tmp->getTile(layer, gridLeft, gridBottom);
											if (tmpTile == nullptr || tmpTile->canCollide == false){
												tmpTile = tmp->getTile(layer, gridLeft, gridBottom - 1);
												if (tmpTile == nullptr || tmpTile->canCollide == false){
													entity->setVelocity(-entity->getVelocity().x, entity->getVelocity().y);
												}
											}
										}
										if (gridRight * tileSize - (posX + sizeXPos) <= 0.0001){
											tmpTile = tmp->getTile(layer, gridRight, gridBottom);
											if (tmpTile == nullptr || tmpTile->canCollide == false){
												tmpTile = tmp->getTile(layer, gridRight, gridBottom - 1);
												if (tmpTile == nullptr || tmpTile->canCollide == false){
													entity->setVelocity(-entity->getVelocity().x, entity->getVelocity().y);
												}
											}

										}
									}
								}
							}
						}
					}
					else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
						entity->setIsActive(false);
						return 0;
					}
					else if (tmpTile->tileType == CHECKPOINT){
						entity->checkPoint();
						return 0;
					}
				}
			}
		}
		return penetration;
	}
	return 0;
}

float GameEngine::checkTopTileCollisions(CompositeEntity* entity){
	if (entity->getIsActive()){
		float rS = sin(entity->getRotation());
		float rC = cos(entity->getRotation());

		float penetration = 0;
		float posX = entity->getPosition().x;
		float posY = entity->getPosition().y;
		float sizeXNeg = abs(entity->getSizeNegative().x * rC - entity->getSizeNegative().y * rS);
		float sizeXPos = abs(entity->getSizePositive().x * rC + entity->getSizePositive().y * rS);
		float sizeY = abs(entity->getSizePositive().y * rC + entity->getSizePositive().x * rS);
		float tileSize = levels[currentLevel]->getTileSize();
		int mapHeightTiles = (int)(mapHeight / tileSize);
		int gridLeft = (int)((posX - sizeXNeg) / tileSize);
		int gridRight = (int)((posX + sizeXPos) / tileSize);
		int gridTop = abs((int)((posY + sizeY) / tileSize));
		int oneBelowGridTop = gridTop + 1;
		unsigned layer = entity->getObjectLayer();
		Level* tmp = levels[currentLevel];
		Tile* tmpTile;
		Tile* tmpTile2;

		if (gridTop < mapHeightTiles && posY + sizeY < 0){
			for (int i = gridLeft; i <= gridRight; i++){
				tmpTile = tmp->getTile(layer, i, gridTop);
				tmpTile2 = tmp->getTile(layer, i, oneBelowGridTop);
				if (tmpTile != nullptr){
					if ((tmpTile->tileType == SOLID || tmpTile->tileType == SOLID_GRASS || tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL) && tmpTile->canCollide && (tmpTile2 == nullptr || !((tmpTile2->tileType == SOLID_FLOOR || tmpTile2->tileType == SOLID_PLATFORM || tmpTile2->tileType == SOLID || tmpTile2->tileType == SOLID_GRASS) && tmpTile2->canCollide))){
						if (tileCollisionUp(posY, sizeY, abs(entity->getSizeNegative().y * rC - entity->getSizePositive().x * rS), gridTop, tmpTile->y, tmpTile->height)){
							//Check if object is below half-point of the tile
							if (getTilePenetrationUp(posY, sizeY, gridTop, tmpTile->y, tmpTile->height) > penetration && getTilePenetrationUp(posY, sizeY, gridTop, tmpTile->y, tmpTile->height) < tileSize && entity->getVelocity().y > 0){
								penetration = getTilePenetrationUp(posY, sizeY, gridTop, tmpTile->y, tmpTile->height);
							}
						}
					}


					else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
						entity->setIsActive(false);
						return 0;
					}
					else if (tmpTile->tileType == CHECKPOINT){
						entity->checkPoint();
						return 0;
					}
				}
			}
		}
		return penetration;
	}
	return 0;
}

float GameEngine::checkLeftTileCollisions(CompositeEntity* entity){
	float rC = cos(entity->getRotation());
	float rS = sin(entity->getRotation());
	float penetration = 0;
	float posX = entity->getPosition().x;
	float posY = entity->getPosition().y;
	float sizeX = abs(entity->getSizeNegative().x * rC - entity->getSizeNegative().y * rS);
	float sizeYPos = abs(entity->getSizePositive().y * rC + entity->getSizePositive().x * rS);
	float sizeYNeg = abs(entity->getSizeNegative().y * rC - entity->getSizeNegative().x * rS);
	float tileSize = levels[currentLevel]->getTileSize();
	int gridLeft = (int)((posX - sizeX) / tileSize);
	int gridBottom = abs((int)((posY - sizeYNeg) / tileSize));
	int gridTop = abs((int)((posY + sizeYPos) / tileSize));
	unsigned layer = entity->getObjectLayer();

	Level* tmp = levels[currentLevel];
	Tile* tmpTile;
	for (int i = gridTop; i <= gridBottom; i++){
		tmpTile = tmp->getTile(layer, gridLeft, i);
		if (tmpTile != nullptr){
			if ((tmpTile->tileType == SOLID || tmpTile->tileType == SOLID_GRASS || tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL) && tmpTile->canCollide){
				if (tileCollisionLeft(posX, sizeX, abs(entity->getSizePositive().x * rC + entity->getSizePositive().y * rS), gridLeft, tmpTile->x, tmpTile->width)){
					if (getTilePenetrationLeft(posX, sizeX, gridLeft, tmpTile->x, tmpTile->width) > penetration){
						penetration = getTilePenetrationLeft(posX, sizeX, gridLeft, tmpTile->x, tmpTile->width);
					}
				}
			}

			else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
				entity->setIsActive(false);
				return 0;
			}
			else if (tmpTile->tileType == CHECKPOINT){
				entity->checkPoint();
				return 0;
			}
		}
	}
	return penetration;
}

float GameEngine::checkRightTileCollisions(CompositeEntity* entity){
	float rS = sin(entity->getRotation());
	float rC = cos(entity->getRotation());

	float posX = entity->getPosition().x;
	float posY = entity->getPosition().y;
	float sizeX = abs(entity->getSizePositive().x * rC + entity->getSizePositive().y * rS);
	float sizeYPos = abs(entity->getSizePositive().y * rC + entity->getSizePositive().x * rS);
	float sizeYNeg = abs(entity->getSizeNegative().y * rC - entity->getSizePositive().x * rS);
	float tileSize = levels[currentLevel]->getTileSize();
	int mapHeightTiles = (int)(mapHeight / tileSize);
	int gridRight = (int)((posX + sizeX) / tileSize);
	int gridBottom = abs((int)((posY - sizeYNeg) / tileSize));
	int gridTop = abs((int)((posY + sizeYPos) / tileSize));
	unsigned layer = entity->getObjectLayer();
	float penetration = 0;

	Level* tmp = levels[currentLevel];
	Tile* tmpTile;
	for (int i = gridTop; i <= gridBottom; i++){
		tmpTile = tmp->getTile(layer, gridRight, i);
		if (tmpTile != nullptr){
			if ((tmpTile->tileType == SOLID || tmpTile->tileType == SOLID_GRASS || tmpTile->tileType == SOLID_FLOOR || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL) && tmpTile->canCollide){
				if (tileCollisionRight(posX, sizeX, abs(entity->getSizeNegative().x * rC - entity->getSizeNegative().y * rS), gridRight, tmpTile->x, tmpTile->width)){
					if (getTilePenetrationRight(posX, sizeX, gridRight, tmpTile->x, tmpTile->width) > penetration){
						penetration = getTilePenetrationRight(posX, sizeX, gridRight, tmpTile->x, tmpTile->width);
					}
				}
			}
			else if (tmpTile->tileType == HAZARD || tmpTile->tileType == HAZARD_FIRE || tmpTile->tileType == HAZARD_SPIKE_HORIZONTAL){
				entity->setIsActive(false);
				return 0;
			}
			else if (tmpTile->tileType == CHECKPOINT){
				entity->checkPoint();
				return 0;
			}
		}
	}
	return penetration;
}

void GameEngine::resolveYCollisions(CompositeEntity* entity){
	if (entity->getIsActive()){
		if (entity->atScreenBoundary(0, mapWidth, -(int)mapHeight, 0)){
			entity->boundaryAction(0, mapWidth, -(int)mapHeight, 0);
		}
		if (entity->getIsActive()){
			float posY = entity->getPosition().y;
			float accY = entity->getAcceleration().y;
			float penetrationUp = checkTopTileCollisions(entity);
			float penetrationDown = checkBottomTileCollisions(entity);
			if (entity->getIsActive()){
				float velY = ((penetrationUp <= 0 && penetrationDown > 0) || (penetrationDown <= 0 && penetrationUp > 0)) ? 0 : entity->getVelocity().y;
				if (penetrationUp <= 0 && penetrationDown > 0){
					if (entity->getEntityType() == PLAYER_PROJECTILE || entity->getEntityType() == ENEMY_PROJECTILE){
						entity->destroy(true, true);
						return;
					}
					posY += penetrationDown;
					entity->setOnTileGround(true);
					if (entity->getVelocity().y > -entity->getJumpSpeed() / 2 && entity->getVelocity().y <= -5 && entity->getJumpSpeed() != 0){
						entity->playCollisionSound(DOWN, WEAK);
					}
					else if ((entity->getVelocity().y > -entity->getJumpSpeed() && entity->getVelocity().y <= -entity->getJumpSpeed() / 2) && entity->getJumpSpeed() != 0){
						entity->playCollisionSound(DOWN, MEDIUM);
					}
					else if (entity->getVelocity().y <= -entity->getJumpSpeed() && entity->getJumpSpeed() != 0){
						entity->hardCollision(DOWN, 4);
						shakeScreen = true;
						shakeDirection = VERTICAL;
						collisionMagnitude = 1;
						shakeCount = 0;
					}
					accY = 0;
				}
				else if (penetrationDown <= 0 && penetrationUp > 0){
					if (entity->getEntityType() == PLAYER_PROJECTILE || entity->getEntityType() == ENEMY_PROJECTILE){
						entity->destroy(true, true);
						
						return;
					}
					posY -= penetrationUp;
					entity->setOnTileGround(false);
					accY = 0;
					entity->setState(FALLING);
				}
				else if (penetrationDown > 0 && penetrationUp > 0){
					if (entity->getEntityType() == PLAYER_PROJECTILE || entity->getEntityType() == ENEMY_PROJECTILE){
						entity->destroy(true, true);
						return;
					}
					posY += penetrationDown;
					entity->setOnTileGround(true);
					if (entity->getVelocity().y > -entity->getJumpSpeed() / 2 && entity->getVelocity().y <= -5 && entity->getJumpSpeed() != 0){
						entity->playCollisionSound(DOWN, WEAK);
					}
					else if ((entity->getVelocity().y > -entity->getJumpSpeed() && entity->getVelocity().y <= -entity->getJumpSpeed() / 2) && entity->getJumpSpeed() != 0){
						entity->playCollisionSound(DOWN, MEDIUM);
					}
					else if (entity->getVelocity().y <= -entity->getJumpSpeed() && entity->getJumpSpeed() != 0){
						entity->hardCollision(DOWN, 4);
						shakeScreen = true;
						shakeDirection = VERTICAL;
						collisionMagnitude = 1;
						shakeCount = 0;
					}
					entity->setOnTileGround(true);
					accY = 0;
				}
				else{
					if (!entity->getStaticCollDownFlag()){
						entity->setOnTileGround(false);
					}
				}
				entity->setPosition(entity->getPosition().x, posY);
				entity->setVelocity(entity->getVelocity().x, velY);
				entity->setAcceleration(entity->getAcceleration().x, accY);
			}
		}
	}
}

void GameEngine::resolveXCollisions(CompositeEntity* entity){
	if (entity->getIsActive()){
		if (entity->atScreenBoundary(0, mapWidth, -(int)mapHeight, 0)){
			entity->boundaryAction(0, mapWidth, -(int)mapHeight, 0);
		}
		else{
			float posX = entity->getPosition().x;
			float velX = entity->getVelocity().x;
			float accX = entity->getAcceleration().x;
			float penetrationLeft = checkLeftTileCollisions(entity);
			float penetrationRight = checkRightTileCollisions(entity);
			if (penetrationRight <= 0 && penetrationLeft > 0){
				if (entity->getEntityType() == PLAYER_PROJECTILE || entity->getEntityType() == ENEMY_PROJECTILE){
					entity->destroy(true, true);
					shakeScreen = true;
					collisionMagnitude = 1;
					shakeDirection = HORIZONTAL;
					shakeCount = 0;
					return;
				}
				posX += penetrationLeft;
				velX = entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN ? -velX : 0;
				accX = 0;
				entity->setState(STATIONARY);
			}
			else if (penetrationLeft <= 0 && penetrationRight > 0){
				if (entity->getEntityType() == PLAYER_PROJECTILE || entity->getEntityType() == ENEMY_PROJECTILE){
					entity->destroy(true, true);
					shakeScreen = true;
					collisionMagnitude = 1;
					shakeDirection = HORIZONTAL;
					shakeCount = 0;
					return;
				}
				posX -= penetrationRight;
				velX = entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN ? -velX : 0;
				accX = 0;
			}
			entity->setPosition(posX, entity->getPosition().y);
			entity->setVelocity(velX, entity->getVelocity().y);
			entity->setAcceleration(accX, entity->getAcceleration().y);
		}
	}
}

void GameEngine::checkTileCollisions(){
	//Using layer, get value of the tile that the evaluated entity is currently touching
	//for (size_t i = 0; i < gameEntities[currentLevel].size(); i++){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			if (entity->getIsActive() && entity->getCanCollideWithTiles() && entity->getCanCollide() && entity->getState() != DESTROYING && entity->getEntityType() != STATIC_ENTITY){
				resolveYCollisions(entity);
				resolveXCollisions(entity);
			}
		}
	}
}

void GameEngine::handleInput(const Uint8* input, SDL_Event input2){
	if (input[SDL_SCANCODE_ESCAPE]){
		gameState = GAME_QUIT;
	}

	if (input[SDL_SCANCODE_1]){
		if (gameState == GAME_PLAY){
			setLevel("level_01");
		}
	}

	if (input[SDL_SCANCODE_2]){
		if (gameState == GAME_PLAY){
			setLevel("level_02");
		}
	}

	if (input[SDL_SCANCODE_3]){
		if (gameState == GAME_PLAY){
			setLevel("level_03");
		}
	}

	if (input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getIsActive()){
			bool doMirror = playerEntity->getRotation() < M_PI;
			playerEntity->setDoMirror(doMirror);
			playerEntity->setOverrideMirroring(true);
			if (playerEntity->getVelocity().x <= -playerEntity->getTopSpeed()){
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setVelocity(-playerEntity->getTopSpeed(), playerEntity->getVelocity().y);
			}
			else if (playerEntity->getVelocity().x > 0 && playerEntity->getVelocity().x < 10){
				playerEntity->setVelocity(0, playerEntity->getVelocity().y);
				playerEntity->setAcceleration(-PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
			}
			else{
				playerEntity->setAcceleration(-PLAYER_ACCELERATION * 5, playerEntity->getAcceleration().y);
			}
		}
	}

	if (input[SDL_SCANCODE_RIGHT] && !input[SDL_SCANCODE_LEFT]){
		if (playerEntity->getIsActive()){
			bool doMirror = playerEntity->getRotation() >= M_PI;
			playerEntity->setDoMirror(doMirror);
			playerEntity->setOverrideMirroring(true);
			if (playerEntity->getVelocity().x >= playerEntity->getTopSpeed()){
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setVelocity(playerEntity->getTopSpeed(), playerEntity->getVelocity().y);
			}
			else if (playerEntity->getVelocity().x < 0 && playerEntity->getVelocity().x > -5){
				playerEntity->setVelocity(0, playerEntity->getVelocity().y);
				playerEntity->setAcceleration(PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
			}
			else{
				playerEntity->setAcceleration(PLAYER_ACCELERATION * 5, playerEntity->getAcceleration().y);
			}
		}
	}

	if (!input[SDL_SCANCODE_LEFT] && !input[SDL_SCANCODE_RIGHT]){
		if (playerEntity->getIsActive()){
			bool overRideMirroring = playerEntity->getRotation() >= M_PI;
			playerEntity->setOverrideMirroring(overRideMirroring);
			if (!playerEntity->atScreenBoundary(0, mapWidth, -(int)(mapHeight), 0)){
				if (abs(playerEntity->getVelocity().x) >= 5){
					if (playerEntity->getVelocity().x > 0){
						playerEntity->setAcceleration(-PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
					}
					else if (playerEntity->getVelocity().x < 0){
						playerEntity->setAcceleration(PLAYER_ACCELERATION, playerEntity->getAcceleration().y);
					}
					else{
						playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
					}
				}
				else{
					playerEntity->setVelocity(0, playerEntity->getVelocity().y);
					playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
					playerEntity->setState(STATIONARY);
				}
			}
			else{
				playerEntity->setVelocity(0, playerEntity->getVelocity().y);
				playerEntity->setAcceleration(0, playerEntity->getAcceleration().y);
				playerEntity->setState(STATIONARY);
			}
		}
	}

	if (input[SDL_SCANCODE_Q] && !input[SDL_SCANCODE_E]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotationalVelocity(0.5);
		}
	}

	if (input[SDL_SCANCODE_E] && !input[SDL_SCANCODE_Q]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotationalVelocity(-0.5);
		}
	}

	if (!input[SDL_SCANCODE_Q] && !input[SDL_SCANCODE_E]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotationalVelocity(0);
		}
	}

	if (input[SDL_SCANCODE_W]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotation(0);
		}
	}

	if (input[SDL_SCANCODE_S]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotation(M_PI);
		}
	}

	if (input[SDL_SCANCODE_D]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotation(M_PI / 2);
		}
	}

	if (input[SDL_SCANCODE_A]){
		if (playerEntity->getIsActive()){
			playerEntity->setRotation(3 * M_PI / 2);
		}
	}

	downPressed = input[SDL_SCANCODE_DOWN];

	if (input[SDL_SCANCODE_UP]){
		if (gameState == GAME_PLAY && playerEntity->getIsActive()){
			if (playerEntity->getIsActive() && playerEntity->getOnGround()){
				playerEntity->jump();
			}
		}
	}

	if (input[SDL_SCANCODE_SPACE]){
		if (gameState == GAME_PLAY && playerEntity->getIsActive()){
			CompositeEntity* projectile = playerEntity->fire();
			if (projectile != nullptr){
				addGameEntity(currentLevel, projectile);
			}
		}
	}
	
	if (input[SDL_SCANCODE_R]){
		if (gameState == GAME_PLAY){
			changeState(GAME_END);
		}
	}

	if (input2.type == SDL_KEYDOWN){
		if (gameState == TITLE_SCREEN){
			if (input2.key.keysym.scancode == SDL_SCANCODE_UP || input2.key.keysym.scancode == SDL_SCANCODE_DOWN){
				if (titleSelection == 0){
					titleSelection = 1;
				}
				else{
					titleSelection = 0;
				}
				changeTitleSelection();
			}

			if (input2.key.keysym.scancode == SDL_SCANCODE_RETURN){
				if (titleSelection == 0){
					changeState(GAME_BEGIN);
				}
				else{
					changeState(GAME_QUIT);
				}
			}
		}
	}
}

void GameEngine::deleteFlagged(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (vector<CompositeEntity*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
			if ((*itr2)->getDoDelete()){
				vector<CompositeEntity*>::iterator newItr = itr->second.erase(itr2);
				itr2 = newItr;
				if (itr2 == itr->second.end()){
					break;
				}
			}
		}
	}

	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntitiesRenderingOrder[currentLevel].begin(); itr != gameEntitiesRenderingOrder[currentLevel].end(); itr++){
		for (vector<CompositeEntity*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
			if ((*itr2)->getDoDelete()){
				delete (*itr2);
				(*itr2) = nullptr;
				vector<CompositeEntity*>::iterator newItr = itr->second.erase(itr2);
				itr2 = newItr;
				if (itr2 == itr->second.end()){
					break;
				}
			}
		}
	}
}

void GameEngine::moveAll(unsigned entityType, float x, float y, float z){
	for (CompositeEntity* entity : gameEntities[currentLevel][entityType]){
		float posX = entity->getPosition().x + x;
		float posY = entity->getPosition().y + y;
		float posZ = entity->getPosition().z + z;

		entity->setPosition(posX, posY, posZ);
	}
}

void GameEngine::turnAll(unsigned entityType, bool turnX, bool turnY, bool turnZ){
	for (CompositeEntity* entity : gameEntities[currentLevel][entityType]){
		float velX = turnX ? -entity->getVelocity().x : entity->getVelocity().x;
		float velY = turnY ? -entity->getVelocity().y : entity->getVelocity().y;
		float velZ = turnZ ? -entity->getVelocity().z : entity->getVelocity().z;

		entity->setVelocity(velX, velY, velZ);
	}
}

void GameEngine::run(float elapsed, const Uint8* input, SDL_Event input2, ShaderProgram* program){
	handleInput(input, input2);
	float fixedElapsed = elapsed;
	if (fixedElapsed >= FIXED_TIMESTEP * MAX_TIMESTEP){
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEP;
	}

	while (fixedElapsed >= FIXED_TIMESTEP){
		fixedElapsed -= FIXED_TIMESTEP;
		update(FIXED_TIMESTEP, program);
	}
	update(fixedElapsed, program);
	render(program, elapsed, FRAMES_PER_SECOND);
}

void GameEngine::update(float elapsed, ShaderProgram* program){
	unsigned enemiesKilled = 0;
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			if (entity->getIsActive()){
				entity->move(elapsed, GRAVITY);
				if (entity->atScreenBoundary(0, mapWidth, -(int)(mapHeight), 0)){
					entity->boundaryAction(0, mapWidth, -(int)(mapHeight), 0);
				}
				CompositeEntity* lastProjectile = gameEntities[currentLevel][ENEMY_PROJECTILE].size() != 0 ? gameEntities[currentLevel][ENEMY_PROJECTILE][gameEntities[currentLevel][ENEMY_PROJECTILE].size() - 1] : nullptr;
				CompositeEntity* projectile = entity->getIsActive() ? entity->logic(playerEntity, lastProjectile) : nullptr;
				if (projectile != nullptr){
					addGameEntity(currentLevel, projectile);
				}
			}
			entity->addToTimeSinceFiring(elapsed);
			if ((entity->getEntityType() == ACTOR_ENEMY || entity->getEntityType() == ACTOR_ENEMY_PATROL_TURN) && gameState == GAME_PLAY){
				if (!entity->getIsActive() && (entity->getPosition().x != 0 && entity->getPosition().y != 0 && entity->getPosition().z != 1)){
					enemiesKilled++;
					entity->setPosition(0, 0, 1);
					points += 100;
				}
			}

			if (entity->getEntityType() == ENTITY_COIN && gameState == GAME_PLAY){
				if (!entity->getIsActive() && (entity->getPosition().x != 0 && entity->getPosition().y != 0 && entity->getPosition().z != 1)){
					entity->setPosition(0, 0, 0);
					points += 10;
				}
			}

			entity->runAnimation(elapsed, FRAMES_PER_SECOND);
		}
	}

	for (CompositeEntity* entity : userInterface){
		if (entity->getEntityType() == POINTS_INDICATOR && entity->getIsActive()){
			entity->setDisplayText("Points: " + std::to_string(points));
		}
		entity->runAnimation(elapsed, FRAMES_PER_SECOND);
	}

	if (gameState == GAME_PLAY){
		for (CompositeEntity* entity : gameEntities[currentLevel][ACTOR_ENEMY]){
			if (entity->getIsActive() && entity->atScreenBoundary(0, mapWidth, -(int)(mapHeight), 0)){
				float moveX = entity->getPosition().x > 0 ? -0.02 : 0.02;
				moveAll(ACTOR_ENEMY, moveX, -0.125);
				turnAll(ACTOR_ENEMY, true, false);
				break;
			}
		}
	}

	deleteFlagged();

	collisionCheck(elapsed);
	checkTileCollisions();

	if (gameState == GAME_PLAY && !playerEntity->getIsActive()){
		if (lives > 1){
			lives--;
			//gameEntities[currentLevel][LIFE_ICON_ENTITY][lives]->setIsActive(false);
			playerEntity->setIsActive(true);
			blinkCount = 0;
			blinkTime = BLINK_TIMING;
			playerEntity->setisInvincible(true);
			playerEntity->resetToCheckpoint();
			//deActivateLevelEntities();
			activateLevelEntities();
			//activateLevelEntities();
		}
		else{
			changeState(GAME_END);
		}
	}

	if (blinkCount <= MAX_BLINK_COUNT && gameState == GAME_PLAY){
		if (blinkTime >= BLINK_TIMING){
			blinkTime = 0;
			blinkCount++;
			playerEntity->blink();
		}
		blinkTime += elapsed;
	}
	else if (blinkCount > MAX_BLINK_COUNT && gameState == GAME_PLAY){
		playerEntity->setisInvincible(false);
	}

	gameOver = gameState == GAME_QUIT;
	checkIfShouldWarp();
}

void GameEngine::changeTitleSelection(){
	if (gameState == TITLE_SCREEN){
		for (CompositeEntity* entity : gameEntities[currentLevel][TITLE_TEXT_ENTITY]){
				if (entity->getEntityID() == "Selection01"){
					if (titleSelection == 0){
						entity->setDisplayText(">> Begin Game!");
					}
					else{
						entity->setDisplayText("Begin Game!");
					}
				}

				if (entity->getEntityID() == "Selection02"){
					if (titleSelection == 1){
						entity->setDisplayText(">> Quit");
					}
					else{
						entity->setDisplayText("Quit");
					}
				}
			}
		}
}

void GameEngine::deActivateLevelEntities(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			entity->reset();
			if (entity->getEntityType() != POINTS_INDICATOR){
				entity->setIsActive(false);
			}
		}
	}
}

void GameEngine::activateLevelEntities(){
	for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
		for (CompositeEntity* entity : itr->second){
			if (entity != nullptr){
				entity->reset();
				if (entity->getEntityType() != ACTOR_PLAYER && entity->getEntityType() != ACTOR_ENEMY && entity->getEntityType() != GAME_TEXT_ENTITY && entity->getEntityType() != ICON_ENTITY && entity->getEntityType() != POINTS_INDICATOR && entity->getEntityType() != LIFE_ICON_ENTITY && entity->getEntityType() != STATIC_ENTITY && entity->getEntityType() != ACTOR_ENEMY_PATROL_TURN && entity->getEntityType() != ENTITY_COIN && entity->getEntityType() != STATIC_FALL_ON_COLLDE && entity->getEntityType() != HIDING_ENEMY_FIRE && entity->getEntityType() != WARP_ENTITY){
					entity->setIsActive(false);
				}
				else{
					entity->setIsActive(true);
					entity->setAllRender(true);
					if (entity->getEntityType() == POINTS_INDICATOR){
						entity->setDisplayText(entity->getDisplayText() + ' ' + to_string(points));
					}
					entity->updateBounding();
				}
			}
		}
	}
}

void GameEngine::changeState(unsigned state){
	switch (state){
	case TITLE_SCREEN:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
			for (CompositeEntity* entity : itr->second){
				entity->reset();
				entity->setIsActive(entity->getEntityType() == TITLE_TEXT_ENTITY);
			}
		}
		gameState = TITLE_SCREEN;
		changeTitleSelection();
		break;
	case GAME_BEGIN:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
			for (CompositeEntity* entity : itr->second){
				if (entity != nullptr){
					if (entity->getEntityType() != ACTOR_PLAYER && entity->getEntityType() != ACTOR_ENEMY && entity->getEntityType() != GAME_TEXT_ENTITY && entity->getEntityType() != ICON_ENTITY && entity->getEntityType() != POINTS_INDICATOR && entity->getEntityType() != LIFE_ICON_ENTITY && entity->getEntityType() != STATIC_ENTITY && entity->getEntityType() != ACTOR_ENEMY_PATROL_TURN && entity->getEntityType() != ENTITY_COIN && entity->getEntityType() != WARP_ENTITY){
						entity->setIsActive(false);
					}
					else{
						entity->reset();
						entity->setIsActive(true);
						if (entity->getEntityType() == POINTS_INDICATOR){
							entity->setDisplayText(entity->getDisplayText() + ' ' + to_string(points));
						}
						entity->updateBounding();
					}
				}
			}
		}
		playerEntity->getEntities()->setDoRender(true);
		music->play();
		gameState = GAME_PLAY;
		lives = 3;
		blinkCount = MAX_BLINK_COUNT + 1;
		points = 0;
		break;
	case GAME_PLAY:
		break;
	case GAME_END:
		setLevel("level_01");
		changeState(TITLE_SCREEN);
		music->stop();
		break;
	case GAME_QUIT:
		break;
	default:
		for (unordered_map<unsigned, vector<CompositeEntity*>>::iterator itr = gameEntities[currentLevel].begin(); itr != gameEntities[currentLevel].end(); itr++){
			for (CompositeEntity* entity : itr->second){
				if (entity->getEntityType() != TITLE_TEXT_ENTITY){
					entity->setIsActive(false);
				}
				else{
					entity->reset();
					entity->setIsActive(true);
				}
			}
		}
		gameState = TITLE_SCREEN;
		changeTitleSelection();
		break;
		
	}
}

bool GameEngine::getGameOver(){
	return gameOver;
}

void GameEngine::start(){
	changeState(TITLE_SCREEN);
	music = new GameMusic();
	music->loadFile(RESOURCE_FOLDER"Assets/Sounds/SuperMarioBros.ogg");
	//music->play();
}

void GameEngine::checkIfShouldWarp(){
	for (CompositeEntity* entity : gameEntities[currentLevel][WARP_ENTITY]){
		if (playerEntity->isCollidingSAT(entity, ALL_DIRECTIONS)){
			setLevel(entity->getWarpDestination());
		}
	}
}

void GameEngine::freeMemory(){
	if (levels[currentLevel] != nullptr){
		try{
			levels[currentLevel]->freeMemory();
			delete levels[currentLevel];
			levels[currentLevel] = nullptr;
		}
		catch (char* e){
			levels[currentLevel] = nullptr;
		}
	}
	levels.erase(currentLevel);

	if (playerEntity != nullptr){
		playerEntity = nullptr;
	}

	if (collisionEvents.size() != 0){
		for (std::vector<CollisionListener*>::iterator itr = collisionEvents.begin(); itr != collisionEvents.end(); itr++){
			if (*itr != nullptr){
				try{
					(*itr)->freeMemory();
					delete *itr;
					*itr = nullptr;
				}
				catch (char* e){
					*itr = nullptr;
				}
			}
			itr = collisionEvents.erase(itr);
			if (itr == collisionEvents.end()){
				break;
			}
		}
	}

	if (gameEntities.size() != 0){
		for (std::unordered_map<std::string, std::unordered_map<unsigned, std::vector<CompositeEntity*>>>::iterator itr = gameEntities.begin(); itr != gameEntities.end(); itr++){
			for (std::unordered_map<unsigned, std::vector<CompositeEntity*>>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
				for (std::vector<CompositeEntity*>::iterator itr3 = itr2->second.begin(); itr3 != itr2->second.end(); itr3++){
					if (*itr3 != nullptr){
						try{
							(*itr3)->freeMemory();
							delete *itr3;
							*itr3 = nullptr;
						}
						catch (char* e){
							*itr3 = nullptr;
						}
					}
					itr3 = itr2->second.erase(itr3);
					if (itr3 == itr2->second.end()){
						break;
					}
				}
				itr2->second.clear();
				itr2 = itr->second.erase(itr2);
				if (itr2 == itr->second.end()){
					break;
				}
			}
			itr->second.clear();
			itr = gameEntities.erase(itr);
			if (itr == gameEntities.end()){
				break;
			}
		}
	}
}