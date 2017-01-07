#include "CompositeEntity.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "mathHelper.h"

CompositeEntity::CompositeEntity()
{
}

CompositeEntity::CompositeEntity(CompositeEntity* toCopy){
	deepCopy(toCopy);
}

CompositeEntity::CompositeEntity(Entity* first){
	this->first = first;
	updateBounding();
}

CompositeEntity::CompositeEntity(Entity* first, float firingDelay){
	this->first = first;
	this->firingDelay = firingDelay;
	timeSinceFiring = firingDelay;
	updateBounding();
}

CompositeEntity::~CompositeEntity()
{
	if (first != nullptr){
		first->freeMemory();
	}
}


Entity* CompositeEntity::getEntities(){
	return first;
}

Vector3 CompositeEntity::getPosition(){
	return position;
}

Vector3 CompositeEntity::getVelocity(){
	return velocity;
}

Vector3 CompositeEntity::getAcceleration(){
	return acceleration;
}

Vector3 CompositeEntity::getSizePositive(){
	return sizePositive;
}

Vector3 CompositeEntity::getSizeNegative(){
	return sizeNegative;
}

Vector3 CompositeEntity::getScale(){
	return scale;
}

float CompositeEntity::getRotation(){
	return rotation;
}

float CompositeEntity::getRotationalVelocity(){
	return rotationalVelocity;
}

float CompositeEntity::getTopSpeed(){
	return topSpeed;
}

BOUNDING_TYPE CompositeEntity::getBoundingType(){
	return boundingType;
}

ENTITY_TYPE CompositeEntity::getEntityType(){
	return type;
}

COLLISION_BEHAVIOR CompositeEntity::getCollisionBehavior(){
	return collisionBehavior;
}

BOUNDARY_BEHAVIOR CompositeEntity::getBoundaryBehavior(){
	return boundaryBehavior;
}

TILE_LOGIC_BEHAVIOR CompositeEntity::getTileLogicBehavior(){
	return tileLogicBehavior;
}

STATE CompositeEntity::getState(){
	return state;
}

bool CompositeEntity::getIsActive(){
	return isActive;
}

bool CompositeEntity::getDoDelete(){
	return doDelete;
}

bool CompositeEntity::getCanCollide(){
	return canCollide;
}

bool CompositeEntity::getisInvincible(){
	return isInvincible;
}

bool CompositeEntity::getOnGround(){
	return staticCollideBottom || onTileGround;
}

bool CompositeEntity::getFalls(){
	return falls;
}

const std::string& CompositeEntity::getEntityID(){
	return entityID;
}

unsigned CompositeEntity::getRenderLayer(){
	return renderLayer;
}

unsigned CompositeEntity::getObjectLayer(){
	return objectLayer;
}

bool CompositeEntity::getCollUpFlag(){
	return collideTop;
}
bool CompositeEntity::getCollDownFlag(){
	return collideBottom;
}
bool CompositeEntity::getStaticCollDownFlag(){
	return staticCollideBottom;
}
bool CompositeEntity::getCollLeftFlag(){
	return collideLeft;
}
bool CompositeEntity::getCollRightFlag(){
	return collideRight;
}


void CompositeEntity::setEntities(Entity* first){
	this->first = first;
	updateBounding();
}

void CompositeEntity::setPosition(float x, float y, float z){
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
	if (y >= -289 && y <= -287){
		int a = 5;
	}
}

void CompositeEntity::setStartingPosition(float x, float y, float z){
	this->startingPosition.x = x;
	this->startingPosition.y = y;
	this->startingPosition.z = z;
	position = startingPosition;
}

void CompositeEntity::setVelocity(float x, float y, float z){
	this->velocity.x = x;
	this->velocity.y = y;
	this->velocity.z = z;
}

void CompositeEntity::setStartingVelocity(float x, float y, float z){
	this->startingVelocity.x = x;
	this->startingVelocity.y = y;
	this->startingVelocity.z = z;
	velocity = startingVelocity;
}

void CompositeEntity::setAcceleration(float x, float y, float z){
	this->acceleration.x = x;
	this->acceleration.y = y;
	this->acceleration.z = z;
}

void CompositeEntity::setStartingAcceleration(float x, float y, float z){
	this->startingAcceleration.x = x;
	this->startingAcceleration.y = y;
	this->startingAcceleration.z = z;
	acceleration = startingAcceleration;
}

void CompositeEntity::setScale(float x, float y, float z){
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

void CompositeEntity::setStartingScale(float x, float y, float z){
	this->startingScale.x = x;
	this->startingScale.y = y;
	this->startingScale.z = z;
	scale = startingScale;
}

void CompositeEntity::setFiringDelay(float firingDelay){
	this->firingDelay = firingDelay;
}

void CompositeEntity::setRotation(float rotation){
	this->rotation = rotation;
}

void CompositeEntity::setStartingRotation(float startingRotation){
	this->startingRotation = startingRotation;
	rotation = startingRotation;
}

void CompositeEntity::setRotationalVelocity(float rotationalVelocity){
	this->rotationalVelocity = rotationalVelocity;
}

void CompositeEntity::setTopSpeed(float topSpeed){
	this->topSpeed = topSpeed;
}

void CompositeEntity::setStartingRotationalVelocity(float startingRotationalVelocity){
	this->startingRotationalVelocity = startingRotationalVelocity;
	rotationalVelocity = startingRotationalVelocity;
}

void CompositeEntity::setBoundingType(BOUNDING_TYPE boundingType){
	this->boundingType = boundingType;
}

void CompositeEntity::setEntityType(ENTITY_TYPE type){
	this->type = type;
}

void CompositeEntity::setCollisionBehavior(COLLISION_BEHAVIOR behavior){
	this->collisionBehavior = behavior;
}

void CompositeEntity::setBoundaryBehavior(BOUNDARY_BEHAVIOR behavior){
	boundaryBehavior = behavior;
}

void CompositeEntity::setTileLogicBehavior(TILE_LOGIC_BEHAVIOR behavior){
	tileLogicBehavior = behavior;
}

void CompositeEntity::setState(STATE state){
	if (this->state != state){
		this->state = state;
		switch (state){
		case IDLE:
		case STATIONARY:
			changeAnimation(ANIMATION_IDLE);
			break;
		case MOVING:
			changeAnimation(ANIMATION_RUN);
			break;
		case ACCELERATING:
			changeAnimation(ANIMATION_WALK);
			break;
		case JUMPING:
			changeAnimation(ANIMATION_JUMP);
			break;
		case FALLING:
			changeAnimation(ANIMATION_FALL);
			break;
		case DESTROYING:
			changeAnimation(ANIMATION_DESTROY);
			break;
		case DEACTIVATING:
			changeAnimation(ANIMATION_DEACTIVATE);
			break;
		default:
			changeAnimation(ANIMATION_IDLE);
			break;
		}
	}
}

void CompositeEntity::setIsActive(bool isActive, bool playSound){
	this->isActive = isActive;
}

void CompositeEntity::setCanCollide(bool canCollide){
	this->canCollide = canCollide;
}

void CompositeEntity::setFalls(bool falls){
	this->falls = falls;
}

void CompositeEntity::setEntityID(const std::string& entityID){
	this->entityID = entityID;
}

void CompositeEntity::setisInvincible(bool isInvincible){
	this->isInvincible = isInvincible;
}

void CompositeEntity::setProjectile(CompositeEntity* projectile){
	this->projectile = projectile;
}

void CompositeEntity::setSpacing(float spacing){
	this->spacing = spacing;
}

void CompositeEntity::setSize(float size){
	this->size = size;
}

void CompositeEntity::setRenderLayer(unsigned renderLayer){
	this->renderLayer = renderLayer;
}

void CompositeEntity::setObjectLayer(unsigned objectLayer){
	this->objectLayer = objectLayer;
}

void CompositeEntity::setOnTileGround(bool onTileGround){
	this->onTileGround = onTileGround;
}

void CompositeEntity::setDoMirror(bool doMirror){
	this->doMirror = doMirror;
}

void CompositeEntity::setOverrideMirroring(bool overrideMirroring){
	this->overrideMirroring = overrideMirroring;
}

void CompositeEntity::reset(){
	onTileGround = false;
	position.x = startingPosition.x;
	position.y = startingPosition.y;
	position.z = startingPosition.z;

	velocity.x = startingVelocity.x;
	velocity.y = startingVelocity.y;
	velocity.z = startingVelocity.z;

	acceleration.x = startingAcceleration.x;
	acceleration.y = startingAcceleration.y;
	acceleration.z = startingAcceleration.z;

	scale.x = startingScale.x;
	scale.y = startingScale.y;
	scale.z = startingScale.z;

	checkPointPosition = startingPosition;

	rotation = startingRotation;

	rotationalVelocity = startingRotationalVelocity;
	resetFlags();

	if (type == STATIC_FALL_ON_COLLDE){
		isStatic = true;
		falls = false;
		canCollide = true;
	}
}

void CompositeEntity::updateBounding(){
	if (isActive){
		if (first != nullptr){
			Entity* checking = first;
			if (checking->getDoRender() && checking->getCanCollide()){
				if (boundingType == SQUARE){
					sizePositive.x = 0;
					sizePositive.y = 0;
					sizePositive.z = 1;

					sizeNegative = sizePositive;
					Vector3 currentOffsetScale;
					currentOffsetScale.x = scale.x * checking->getScale().x;
					currentOffsetScale.y = scale.y * checking->getScale().y;
					currentOffsetScale.z = 1;
					if (checking->getDoRender() && checking->getCanCollide()){
						float pos = 0;
						checking->updateBounding(scale, position, rotation);

						pos = checking->getPosition().x;
						if (pos > 0){
							if (pos < 1 && pos > 0){
								sizePositive.x += currentOffsetScale.x * pos;
								sizeNegative.x += currentOffsetScale.x * (1 - pos);
							}
							else if (pos == 1){
								sizePositive.x += currentOffsetScale.x;
							}
							else if (pos > 1){
								sizePositive.x += currentOffsetScale.x * (1 + pos);
							}
						}
						else if (pos < 0){
							if (pos > -1 && pos < 0){
								sizeNegative.x += currentOffsetScale.x * abs(pos);
								sizePositive.x += currentOffsetScale.x * (1 + pos);
							}
							else if (pos == -1){
								sizeNegative.x += currentOffsetScale.x;
							}
							else if (pos < -1){
								sizeNegative.x += currentOffsetScale.x * (1 + abs(pos));
							}
						}
						else if (pos == 0){
							sizePositive.x += currentOffsetScale.x / 2;
							sizeNegative.x += currentOffsetScale.x / 2;
						}

						pos = checking->getPosition().y;
						if (pos > 0){
							if (pos < 1 && pos > 0){
								sizePositive.y += currentOffsetScale.y * (pos);
								sizeNegative.y += currentOffsetScale.y * (1 - pos);
							}
							else if (pos == 1){
								sizePositive.y += currentOffsetScale.y;
							}
							else if (pos > 1){
								sizePositive.y += currentOffsetScale.y * (1 + pos);
							}
						}
						else if (pos < 0){
							if (pos > -1 && pos < 0){
								sizeNegative.y += currentOffsetScale.y * abs(pos);
								sizePositive.y += currentOffsetScale.y * (1 + pos);
							}
							else if (pos == -1){
								sizeNegative.y += currentOffsetScale.y;
							}
							else if (pos < -1){
								sizeNegative.y += currentOffsetScale.y * (1 + abs(pos));
							}
						}
						else if (pos == 0){
							sizePositive.y += currentOffsetScale.y / 2;
							sizeNegative.y += currentOffsetScale.y / 2;
						}
					}
				}
			}
			if (checking->getSibling() != nullptr){
				updateBoundingRecurse(checking->getSibling(), scale);
			}
			if (checking->getChild() != nullptr){
				updateBoundingRecurse(checking->getChild(), scale, checking->getPosition().x, checking->getPosition().y, checking->getPosition().z);
			}
		}
		if (type == WARP_ENTITY){
			sizePositive.x = 8;
			sizePositive.y = 8;
			sizePositive.z = 0;
			sizeNegative = sizePositive;
		}

		if ((sizePositive.x != sizeNegative.x || sizePositive.y != sizeNegative.y || sizePositive.z != sizeNegative.z) && (type != GAME_TEXT_ENTITY && type != POINTS_INDICATOR && type != TITLE_TEXT_ENTITY)){
			centralize();
			centralized = true;
		}
		projectToAxis();
	}
}

void CompositeEntity::updateBoundingRecurse(Entity* check, Vector3 offsetScale, float offsetX, float offsetY, float offsetZ){
	Vector3 currentOffsetScale;
	if (check != nullptr){
		currentOffsetScale.x = offsetScale.x * check->getScale().x;
		currentOffsetScale.y = offsetScale.y * check->getScale().y;
		currentOffsetScale.z = 1;
		if (check->getDoRender() && check->getCanCollide()){
			float pos = 0;
			check->updateBounding(scale, position, rotation);
			//if (offsetX + check->getPosition().x == 0 && offsetY + check->getPosition().y == 0 && offsetZ + check->getPosition().z == 0){
			//	throw "Two entities should not be in the same spot!!";
			//}
			//Need to check that it can still collide:
			if (check->getCanCollide()){
				pos = check->getPosition().x;
				if (offsetX + pos > 0){
					if (pos == 0){
						if (pos > sizePositive.x){
							sizePositive.x += offsetScale.x;
						}
					}
					else if (pos < 1 && pos > 0){
						sizePositive.x += currentOffsetScale.x * pos;
					}
					else if (pos == 1){
						sizePositive.x += currentOffsetScale.x;
					}
					else if (pos > 1){
						sizePositive.x += currentOffsetScale.x * (1 + pos);
					}
				}
				else if (offsetX + pos < 0){
					if (pos == 0){
						if (pos > sizeNegative.x){
							sizeNegative.x += sizeNegative.x - check->getSize().x / 2;
						}
					}
					else if (pos > -1 && pos < 0){
						sizeNegative.x += currentOffsetScale.x * abs(pos);
					}
					else if (pos == -1){
						sizeNegative.x += currentOffsetScale.x;
					}
					else if (pos < -1){
						sizeNegative.x += currentOffsetScale.x * (1 + abs(pos));
					}
				}
				else if (offsetX + pos == 0){
					if (pos < 0){
						if (pos > -1 && pos < 0){
							sizeNegative.x += currentOffsetScale.x * abs(pos);
						}
						else if (pos == -1){
							sizeNegative.x += currentOffsetScale.x;
						}
						else if (pos < -1){
							sizeNegative.x += currentOffsetScale.x * (1 + abs(pos));
						}
					}
					else if (pos > 0){
						if (pos < 1 && pos > 0){
							sizePositive.x += currentOffsetScale.x * pos;
						}
						else if (pos == 1){
							sizePositive.x += currentOffsetScale.x;
						}
						else if (pos > 1){
							sizePositive.x += currentOffsetScale.x * (1 + pos);
						}
					}
				}


				pos = check->getPosition().y;
				if (pos < 0){
					if (pos > -1 && pos < 0){
						sizeNegative.y += currentOffsetScale.y * abs(pos);
					}
					else if (pos == -1){
						sizeNegative.y += currentOffsetScale.y;
					}
					else if (pos < -1){
						sizeNegative.y += currentOffsetScale.y * (1 + abs(pos));
					}
				}
				else if (pos > 0){
					if (pos < 1 && pos > 0){
						sizePositive.y += currentOffsetScale.y * (pos);
					}
					else if (pos == 1){
						sizePositive.y += currentOffsetScale.y;
					}
					else if (pos > 1){
						sizePositive.y += currentOffsetScale.y * (1 + pos);
					}
				}

				//	pos = check->getPosition().z;
				//	if (offsetZ + pos > 0){
				//		if (pos == 0){
				//			if (pos > sizePositive.z){
				//				sizePositive.z += sizePositive.z - check->getSize().z / 2;
				//			}
				//		}
				//		else if (pos < 1 && pos > 0){
				//			sizePositive.z += check->getSize().z / 2 - pos;
				//		}
				//		else if (pos == 1){
				//			sizePositive.z += check->getSize().z / 2;
				//		}
				//		else if (pos > 1){
				//			sizePositive.z += check->getSize().z / 2 + pos;
				//		}
				//	}
				//	else if (offsetZ + pos < 0){
				//		if (pos == 0){
				//			if (pos > sizeNegative.z){
				//				sizeNegative.z += sizeNegative.z - check->getSize().z / 2;
				//			}
				//		}
				//		else if (pos < 1 && pos > 0){
				//			sizeNegative.z += check->getSize().z / 2 - pos;
				//		}
				//		else if (pos == 1){
				//			sizeNegative.z += check->getSize().z / 2;
				//		}
				//		else if (pos > 1){
				//			sizeNegative.z += check->getSize().z / 2 + pos;
				//		}
				//	}
			}
		}
		sizePositive.z = 1;
		sizeNegative.z = 1;
		updateBoundingRecurse(check->getChild(), currentOffsetScale, check->getPosition().x + offsetX, check->getPosition().y + offsetY, check->getPosition().z + offsetZ);
		updateBoundingRecurse(check->getSibling(), offsetScale, offsetX, offsetY, offsetZ);
	}
}

void CompositeEntity::centralize(){
	Vector3 totalSize;
	Vector3 offset;

	Vector3 oldSizePositive = sizePositive;
	
	totalSize.x = sizePositive.x + sizeNegative.x;
	totalSize.y = sizePositive.y + sizeNegative.y;
	totalSize.z = sizePositive.z + sizeNegative.z;

	sizePositive.x = totalSize.x / 2;
	sizePositive.y = totalSize.y / 2;
	sizePositive.z = totalSize.z / 2;

	sizeNegative = sizePositive;

	if (!centralized){
		offset.x = oldSizePositive.x - totalSize.x / 2;
		offset.y = oldSizePositive.y - totalSize.y / 2;
		offset.z = oldSizePositive.z - totalSize.z / 2;

		position.x += offset.x;
		position.y += offset.y;
		position.z += offset.z;

		startingPosition.x += offset.x;
		startingPosition.y += offset.y;
		startingPosition.z += offset.z;

		offset.x /= scale.x;
		offset.y /= scale.y;
		offset.z /= scale.z;
		if (abs(offset.x) < 1 && offset.x != 0){
			offset.x = 1 - abs(offset.x);
		}
		if (abs(offset.y) < 1 && offset.y != 0){
			offset.y = 1 - abs(offset.y);
		}
		if (abs(offset.z) < 1 && offset.z != 0){
			offset.z = 1 - abs(offset.z);
		}
		//offset.x *= first->getNumOfEntities(true);
		//offset.y *= first->getNumOfEntities(true);
		//offset.z *= first->getNumOfEntities(true);
		first->centralize(offset);

		if (particleEmitters.size() != 0){
			for (std::unordered_map<unsigned, std::vector<ParticleEmitter*>>::iterator itr = particleEmitters.begin(); itr != particleEmitters.end(); itr++){
				for (ParticleEmitter* emitter : itr->second){
					emitter->position.x -= offset.x * 2;
					emitter->position.y -= offset.y * 2;
				}
			}
		}
	}
}

bool CompositeEntity::subEntitiesColliding(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat, CompositeEntity* that){
	if (firstOfThis->getBoundingType() == SQUARE && firstOfThat->getBoundingType() == SQUARE){
		if (firstOfThis->getCanCollide() && firstOfThat->getCanCollide()){
			if (!(position.x + firstOfThis->getPosition().x + firstOfThis->getSize().x/2 < that->position.x + firstOfThat->getPosition().x - firstOfThat->getSize().x/2 || position.x + firstOfThis->getPosition().x - sizeNegative.x > that->position.x + firstOfThat->getPosition().x + firstOfThat->getSize().x/2 || position.y + firstOfThis->getPosition().y + firstOfThis->getSize().y/2 < that->position.y + firstOfThat->getPosition().y - firstOfThat->getSize().y/2 || position.y + firstOfThis->getPosition().y - firstOfThis->getSize().y/2 > that->position.y + firstOfThat->getPosition().y + firstOfThat->getSize().y/2)){
				return true;
			}
		}
		if (firstOfThis->getSibling() != nullptr){
			if (subEntitiesColliding(firstOfThis->getSibling(), originalOfThat, originalOfThat, that)){
				return true;
			}
		}
		if (firstOfThis->getChild() != nullptr){
			if (subEntitiesColliding(firstOfThis->getChild(), originalOfThat, originalOfThat, that)){
				return true;
			}
		}

		if (firstOfThat->getSibling() != nullptr){
			if (subEntitiesColliding(firstOfThis, firstOfThat->getSibling(), originalOfThat, that)){
				return true;
			}
		}
		if (firstOfThat->getChild() != nullptr){
			if (subEntitiesColliding(firstOfThis, firstOfThat->getChild(), originalOfThat, that)){
				return true;
			}
		}
	}
	return false;
}

bool CompositeEntity::subEntitiesCollidingSAT(Entity* firstOfThis, Entity* firstOfThat, Entity* originalOfThat, CompositeEntity* that){
	if (firstOfThis->getBoundingType() == SQUARE && firstOfThat->getBoundingType() == SQUARE){
		if (firstOfThis->getCanCollide() && firstOfThat->getCanCollide()){
			if (checkSATCollision(firstOfThis->getSATCoordinates(), firstOfThat->getSATCoordinates())){
				collisionNode = firstOfThis;
				otherCollisionNode = firstOfThat;
				return true;
			}
		}
		if (firstOfThis->getSibling() != nullptr){
			if (subEntitiesCollidingSAT(firstOfThis->getSibling(), originalOfThat, originalOfThat, that)){
				return true;
			}
		}
		if (firstOfThis->getChild() != nullptr){
			if (subEntitiesCollidingSAT(firstOfThis->getChild(), originalOfThat, originalOfThat, that)){
				return true;
			}
		}

		if (firstOfThat->getSibling() != nullptr){
			if (subEntitiesCollidingSAT(firstOfThis, firstOfThat->getSibling(), originalOfThat, that)){
				return true;
			}
		}
		if (firstOfThat->getChild() != nullptr){
			if (subEntitiesCollidingSAT(firstOfThis, firstOfThat->getChild(), originalOfThat, that)){
				return true;
			}
		}
	}
	collisionNode = nullptr;
	otherCollisionNode = nullptr;
	return false;
}

bool CompositeEntity::atScreenBoundary(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling){
	if (state == DEACTIVATING || state == DESTROYING){
		return false;
	}
	bool atBoundary = (position.x + sizePositive.x * cos(rotation) + sizePositive.y * sin(rotation) >= gameWallRight || position.x - (sizeNegative.x * cos(rotation) - sizeNegative.y * sin(rotation)) <= gameWallLeft || position.y + sizePositive.y * cos(rotation) + sizePositive.x * sin(rotation) >= gameCeiling || position.y - (sizeNegative.y * cos(rotation) - sizeNegative.y * sin(rotation)) <= gameFloor);
	if (atBoundary){
		boundaryLeft = position.x - abs((sizeNegative.x * cos(rotation) - sizeNegative.y * sin(rotation))) <= gameWallLeft;
		boundaryRight = position.x + abs(sizePositive.x * cos(rotation) + sizePositive.y * sin(rotation)) >= gameWallRight;
		boundaryTop = position.y + abs(sizePositive.y * cos(rotation) + sizePositive.x * sin(rotation)) >= gameCeiling;
		boundaryBottom = position.y - abs((sizeNegative.y * cos(rotation) - sizeNegative.y * sin(rotation))) <= gameFloor;
	}
	else{
		boundaryLeft = false;
		boundaryRight = false;
		boundaryTop = false;
		boundaryBottom = false;
	}

	return atBoundary;
}

bool CompositeEntity::getAtBottom(){
	return boundaryBottom;
}

bool CompositeEntity::isColliding(CompositeEntity* collidingWith){
	updateBounding();
	collidingWith->updateBounding();
	bool isColliding = false;
	if (canCollide && collidingWith->getCanCollide() && isActive && collidingWith->getIsActive()){
		if (objectLayer == collidingWith->objectLayer){
			if (boundingType == SQUARE && collidingWith->boundingType == SQUARE){
				if (!(position.x + sizePositive.x < collidingWith->position.x - collidingWith->sizeNegative.x ||
					position.x - sizeNegative.x > collidingWith->position.x + collidingWith->sizePositive.x ||
					position.y + sizePositive.y  < collidingWith->position.y - collidingWith->sizeNegative.y ||
					position.y - sizeNegative.y > collidingWith->position.y + collidingWith->sizePositive.y)){
					isColliding = (this->first == nullptr || collidingWith->first == nullptr) || subEntitiesColliding(first, collidingWith->first, collidingWith->first, collidingWith);
					if (isColliding){
						collideLeft = position.x - sizeNegative.x >= collidingWith->position.x - collidingWith->sizeNegative.x;

						collideRight = position.x + sizePositive.x <= collidingWith->position.x + collidingWith->sizePositive.x;

						collideBottom = position.y - sizeNegative.y >= collidingWith->position.y - collidingWith->sizeNegative.y;

						collideTop = position.y + sizePositive.y <= collidingWith->position.y + collidingWith->sizePositive.y;

						if (collideLeft && collideRight){
							collideRight = false;
							collideLeft = false;
						}
						if (collideTop && collideBottom){
							collideTop = false;
							collideBottom = false;
						}
					}
					else{
						collideLeft = false;
						collideRight = false;
						collideTop = false;
						collideBottom = false;
						staticCollideLeft = false;
						staticCollideRight = false;
						staticCollideTop = false;
						staticCollideBottom = false;
					}
				}
			}
		}
	}
	return isColliding;
}

bool CompositeEntity::isCollidingSAT(CompositeEntity* collidingWith, DIRECTION collisionDirection){
	//switch (direction){
	//case ALL_DIRECTIONS:
	//	return true;
	//	break;
	//case UP:
	//	return e1V.y - e2V.y > 0;
	//	break;
	//case LEFT:
	//	return e1V.x - e2V.x < 0;
	//	return false;
	//	break;
	//case RIGHT:
	//	return e1V.x - e2V.x > 0;
	//	break;
	//case DOWN:
	//	return e1V.y - e2V.y < 0;
	//	break;
	//default:
	//	throw "Unknown direction!";
	//	break;
	//}
	if ((!canCollide || state == DEACTIVATING || state == DESTROYING || !isActive) || (!collidingWith->canCollide || collidingWith->state == DEACTIVATING || collidingWith->state == DESTROYING || !collidingWith->isActive)){
		return false;
	}
	collideTop = false;
	collideLeft = false;
	collideRight = false;
	collideBottom = false;
	staticCollideLeft = false;
	staticCollideRight = false;
	staticCollideTop = false;
	staticCollideBottom = staticCollideBottom && velocity.y == 0;
	Vector3 e1V = velocity;
	Vector3 e2V = collidingWith->velocity;
	switch (collisionDirection){
	case ALL_DIRECTIONS:
		if (checkSATCollisionDirectional(SATcoordinates, collidingWith->SATcoordinates, e1V, e2V, ALL_DIRECTIONS)){
			if (subEntitiesCollidingSAT(first, collidingWith->first, collidingWith->first, collidingWith)){
				collideTop = e1V.y - e2V.y > 0;
				collideLeft = e1V.x - e2V.x < 0;
				collideRight = e1V.x - e2V.x > 0;
				collideBottom = e1V.y - e2V.y < 0;

				bool boundLeft = position.x >= collidingWith->position.x - collidingWith->sizeNegative.x;

				bool boundRight = position.x <= collidingWith->position.x + collidingWith->sizePositive.x;

				bool boundBottom = position.y >= collidingWith->position.y - collidingWith->sizeNegative.y;

				bool boundTop = position.y <= collidingWith->position.y + collidingWith->sizePositive.y;
				
				bool halfHeight = position.y - sizeNegative.y >= collidingWith->position.y;

				if (boundLeft && boundRight){
					collideLeft = false;
					collideRight = false;
				}

				if (boundTop && boundBottom){
					collideTop = false;
					collideBottom = false;
				}

				if (collideTop && (boundLeft || boundRight) && !halfHeight){
					collideTop = false;
				}
				
				if (halfHeight){
					collideLeft = false;
					collideRight = false;
				}

				if (collidingWith->type == STATIC_FALL_ON_COLLDE && collidingWith->state == FALLING){
					collideLeft = false;
					collideRight = false;
					collideTop = false;
				}
				return collideLeft || collideRight || collideTop || collideBottom;
			}
		}
		return false;
		break;
	case LEFT:
		if (checkSATCollisionDirectional(getSATCoordinates(), collidingWith->getSATCoordinates(), e1V, e2V, LEFT)){
			collideLeft = (subEntitiesCollidingSAT(first, collidingWith->first, collidingWith->first, collidingWith));
		}
		return collideLeft;
		break;
	case RIGHT:
		if (checkSATCollisionDirectional(getSATCoordinates(), collidingWith->getSATCoordinates(), e1V, e2V, RIGHT)){
			collideRight = subEntitiesCollidingSAT(first, collidingWith->first, collidingWith->first, collidingWith);
		}
		return collideRight;
		break;
	case UP:
		if (checkSATCollisionDirectional(getSATCoordinates(), collidingWith->getSATCoordinates(), e1V, e2V, UP)){
			collideTop = subEntitiesCollidingSAT(first, collidingWith->first, collidingWith->first, collidingWith);
		}
		return collideTop;
		break;
	case DOWN:
		if (checkSATCollisionDirectional(getSATCoordinates(), collidingWith->getSATCoordinates(), e1V, e2V, DOWN)){
			collideBottom = subEntitiesCollidingSAT(first, collidingWith->first, collidingWith->first, collidingWith);
		}
		return collideBottom;
		break;
	default:
		throw "Unknown direction!";
		break;
	}
}


void CompositeEntity::move(float elapsed, float gravity, float frictionX, float frictionY){
	resetFlags();
	static float elapsedY;
	static float elapsedX;
	if (!isStatic &&  isActive && state != DESTROYING && state != DEACTIVATING && !doDelete){

		//velocity.x = lerp(velocity.x, 0.0f, elapsed * frictionX);
		//velocity.y = lerp(velocity.y, 0.0f, elapsed * frictionY);
		
		//velocity.x = lerp(velocity.x, acceleration.x, elapsed);
		//velocity.y = lerp(velocity.y, acceleration.y, elapsed);

		velocity.x += acceleration.x * elapsed;
		velocity.y += acceleration.y * elapsed;
		velocity.z += acceleration.z * elapsed;

		position.x += velocity.x * elapsed;
		position.y += velocity.y * elapsed;
		position.z += velocity.z * elapsed;
		//position.x = easeIn(position.x, position.x + velocity.x * elapsed, elapsed);
		//position.y = easeIn(position.y, position.y + velocity.y * elapsed, elapsed);
		//position.z = easeIn(position.z, position.z + velocity.z * elapsed, elapsed);

		if (falls && !onTileGround && !staticCollideBottom){
			velocity.y += gravity * elapsed;
		}
		rotation += rotationalVelocity * elapsed;
		doMirror = doMirror;
		if (!overrideMirroring){
			doMirror = velocity.x < 0 || (velocity.x == 0 && doMirror);
		}
	
		
		if (first != nullptr){
			first->move(elapsed);
		}
		if ((acceleration.x != 0 || acceleration.y != 0 || acceleration.z != 0) && state != JUMPING && velocity.y >= -10){
			setState(ACCELERATING);
		}
		else if (velocity.x != 0 && state != JUMPING && velocity.y >= -10){
			setState(MOVING);
		}
		else if (velocity.y > 0){
			setState(JUMPING);
		}
		else if ((velocity.y < 0 && state == JUMPING) || velocity.y < -10){
			setState(FALLING);
		}
		else{
			setState(STATIONARY);
		}
	}
	else if (state == DESTROYING && first->animationComplete(ANIMATION_DESTROY)){
		doDelete = true;
	}
	else if (state == DEACTIVATING && first->animationComplete(ANIMATION_DEACTIVATE)){
		state = IDLE;
		isActive = false;
	}
	if (particleEmitters.size() != 0){
		for (std::unordered_map<unsigned, std::vector<ParticleEmitter*>>::iterator itr = particleEmitters.begin(); itr != particleEmitters.end(); itr++){
			for (ParticleEmitter* emitter : itr->second){
				emitter->update(elapsed, gravity, velocity, position);
			}
		}
	}
	if (type == STATIC_FALL_ON_COLLDE && velocity.y != 0){
		state = FALLING;
	}
	updateBounding();
}

void CompositeEntity::jump(){
	velocity.y = jumpSpeed;
	setState(JUMPING);
	playTriggerSound(TRIG_JUMP);
}

void CompositeEntity::drawText(ShaderProgram* program, Matrix matrix, float elapsed, float fps){
	static std::vector<float> color;
	float texture_size = 1.0 / 16.0f; 
	std::vector<float> vertexData; 
	std::vector<float> texCoordData;
	Matrix offsetMatrix;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f * size, ((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y,
			texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
		});
	}
	if (type == TITLE_TEXT_ENTITY || isStatic){
		transformMatrix();
		program->setModelMatrix(modelMatrix);
		offsetMatrix = modelMatrix;
	}
	else{
		program->setModelMatrix(matrix);
		offsetMatrix = matrix;
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	if (color.size() == 0){
		for (int i = 0; i < vertexData.size()/2; i++){
			color.insert(color.end(), { 1.0, 1.0, 1.0, 1.0 });
		}
	}


	GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, color.data());
	glEnableVertexAttribArray(colorAttribute);

	glBindTexture(GL_TEXTURE_2D, textSheet);
	glDrawArrays(GL_TRIANGLES, 0, vertexData.size()/2);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(colorAttribute);

	if (first != nullptr){
		first->draw(program, offsetMatrix, elapsed, fps);
	}
}

void CompositeEntity::draw(ShaderProgram* program, Matrix matrix, float elapsed, float fps){
	switch (type){
	case GAME_TEXT_ENTITY:
	case TITLE_TEXT_ENTITY:
	case POINTS_INDICATOR:
		drawText(program, matrix, elapsed, fps);
		break;
	case WARP_ENTITY:
		break;
	default:
		transformMatrix();
		first->draw(program, modelMatrix, elapsed, fps);
		//first->draw(program, position, scale, rotation, elapsed, fps);
		break;
	}
	if (particleEmitters.size() != 0){
		for (std::unordered_map<unsigned, std::vector<ParticleEmitter*>>::iterator itr = particleEmitters.begin(); itr != particleEmitters.end(); itr++){
			for (ParticleEmitter* emitter: itr->second){
				emitter->render(program);
			}
		}
	}
}

void CompositeEntity::bounce(float elapsed, CompositeEntity* bouncingOffOf){
	if (collideLeft){
		position.x = bouncingOffOf->position.x + sizePositive.x/2;
		if (velocity.x < 0){
			velocity.x *= -0.8;
			if (abs(velocity.x) <= 1){
				collisionStrength = WEAK;
			}
			else if (abs(velocity.x) > 1 && abs(velocity.x) <= 5){
				collisionStrength = MEDIUM;
			}
			else if (abs(velocity.x) > 5){
				collisionStrength = STRONG;
			}
			else{
				collisionStrength = WEAK;
			}
		}
		//collideLeft = velocity.x == 0;
	}
	if (collideRight){
		position.x = bouncingOffOf->position.x - sizeNegative.x/2;
		if (velocity.x > 0){
			velocity.x *= -0.8;
			if (abs(velocity.x) <= 1){
				collisionStrength = WEAK;
			}
			else if (abs(velocity.x) > 1 && abs(velocity.x) <= 5){
				collisionStrength = MEDIUM;
			}
			else if (abs(velocity.x) > 5){
				collisionStrength = STRONG;
			}
			else{
				collisionStrength = WEAK;
			}
		}
		//collideRight = velocity.x == 0;
	}
	if (collideTop){
		position.y = bouncingOffOf->position.y - sizeNegative.y/2;
		if (velocity.y > 0){
			velocity.y *= -0.8;
			if (abs(velocity.y) <= 1){
				collisionStrength = WEAK;
			}
			else if (abs(velocity.y) > 1 && abs(velocity.y) <= 5){
				collisionStrength = MEDIUM;
			}
			else if (abs(velocity.y) > 5){
				collisionStrength = STRONG;
			}
			else{
				collisionStrength = WEAK;
			}
		}
		//collideTop = velocity.y == 0;
	}
	if (collideBottom){
		position.y = bouncingOffOf->position.y + sizePositive.y/2;
		if (velocity.y < 0){
			velocity.y *= -0.8;
			if (abs(velocity.y) <= 1){
				collisionStrength = WEAK;
			}
			else if (abs(velocity.y) > 1 && abs(velocity.y) <= 5){
				collisionStrength = MEDIUM;
			}
			else if (abs(velocity.y) > 5){
				collisionStrength = STRONG;
			}
			else{
				collisionStrength = WEAK;
			}
		}
		//collideBottom = velocity.y == 0;
	}
	setState(BOUNCED);
}

void CompositeEntity::fall(){
	canCollideWithTiles = false;
	isStatic = false;
	falls = true;
	velocity.y = 5;
	state = FALLING;
}

void CompositeEntity::playCollisionSound(DIRECTION direction, COLLISION_STRENGTH collisionStrength){
	if (collisionSounds.size() != 0){
		COLLISION_STRENGTH strengthToUse = this->collisionStrength;
		DIRECTION directionToUse = DIRECTION_SIZE;
		if (collisionStrength != COLLISION_STRENGTH_SIZE){
			strengthToUse = collisionStrength;
		}
		if (direction != DIRECTION_SIZE){
			directionToUse = direction;
		}
		else{
			if (collideTop){
				directionToUse = UP;
			}
			else if (collideBottom){
				directionToUse = DOWN;
			}
			else if (collideLeft){
				directionToUse = LEFT;
			}
			else if (collideRight){
				directionToUse = RIGHT;
			}
			else{
				directionToUse = DOWN;
			}
		}
		if (collisionSounds.find(strengthToUse) != collisionSounds.end() && collisionSounds[strengthToUse].find(directionToUse) != collisionSounds[strengthToUse].end()){
			if (!(directionToUse == DOWN && staticCollideBottom && bottomStaticCollisionCount > 1)){
				collisionSounds[strengthToUse][directionToUse]->play();
			}
		}
	}
}

void CompositeEntity::playTriggerSound(SOUND_TRIGGER trigger){
	if (triggerSounds.size() != 0 && triggerSounds.find(trigger) != triggerSounds.end()){
		triggerSounds[trigger]->play();
	}
}

void CompositeEntity::postCollisionAction(GameSound* sound){
	if (sound == nullptr){
		playCollisionSound();
	}
	else{
		sound->play();
	}
}

void CompositeEntity::stop(){
	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;

	acceleration.x = 0;
	acceleration.y = 0;
	acceleration.z = 0;

	setState(STATIONARY);
}

void CompositeEntity::destroy(bool doAnimate, bool playSound){
	if (doAnimate && (first->getIsAnimated() && first->hasAnimation(ANIMATION_DESTROY))){
		setState(DESTROYING);
	}
	else{
		doDelete = true;
	}
	if (playSound)
		playTriggerSound(TRIG_DESTROY);
}

void CompositeEntity::deActivate(){
	if (first->getIsAnimated() && first->hasAnimation(ANIMATION_DEACTIVATE)){
		setState(DEACTIVATING);
	}
	else{
		isActive = false;
	}
	playTriggerSound(TRIG_DEACTIVATE);
}


void CompositeEntity::boundaryStop(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling){
	if ((boundaryLeft && velocity.x < 0) || (boundaryRight && velocity.x > 0)){
		velocity.x = 0;
		acceleration.x = 0;


		if (boundaryLeft && position.x - sizeNegative.x != gameWallLeft){
			position.x = gameWallLeft + sizePositive.x;
		}

		if (boundaryRight && position.x + sizePositive.x != gameWallRight){
			position.x = gameWallRight - sizeNegative.x;
		}
	}

	if (boundaryTop || boundaryBottom){
		velocity.y = 0;
		acceleration.y = 0;
	}

	if (boundaryBottom && position.y - sizeNegative.y != gameFloor){
		position.y = gameFloor + sizePositive.y;
	}

	if (boundaryTop && position.y + sizePositive.y != gameCeiling){
		position.y = gameCeiling + sizePositive.y;
	}
	setState(STATIONARY);
}

void CompositeEntity::boundaryStopAtWall(float gameWallLeft, float gameWallRight){
	if ((boundaryLeft && velocity.x < 0) || (boundaryRight && velocity.x > 0)){
		velocity.x = 0;
		acceleration.x = 0;


		if (boundaryLeft && position.x - sizeNegative.x != gameWallLeft){
			position.x = gameWallLeft + sizePositive.x;
		}

		if (boundaryRight && position.x + sizePositive.x != gameWallRight){
			position.x = gameWallRight - sizeNegative.x;
		}
	}
}

void CompositeEntity::boundaryTurn(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling){
	if (boundaryLeft || boundaryRight){
		velocity.x = -velocity.x;
		acceleration.x = -acceleration.x;
	}

	if (boundaryLeft && position.x - sizeNegative.x != gameWallLeft){
		position.x = gameWallLeft + sizePositive.x + 0.01;
		boundaryLeft = false;
	}

	if (boundaryRight && position.x + sizePositive.x != gameWallRight){
		position.x = gameWallRight - sizeNegative.x - 0.01;
		boundaryRight = false;
	}

	if (boundaryTop || boundaryBottom){
		velocity.y = -velocity.y;
		acceleration.y = -acceleration.y;
	}

	if (boundaryBottom && position.y - sizeNegative.y/2 != gameFloor){
		position.y = gameFloor + sizePositive.y + 0.01;
		boundaryBottom = false;
	}

	if (boundaryTop && position.y + sizePositive.y != gameCeiling){
		position.y = gameCeiling - sizeNegative.y - 0.01;
		boundaryTop = false;
	}
}

//Throws exception if collision behavior unrecognized
void CompositeEntity::collide(float elapsed, CompositeEntity* collidingWith, COLLISION_BEHAVIOR collisionBehavior, GameSound* sound){
	switch (collisionBehavior){
	case BOUNCE:
		state = BOUNCING;
		bounce(elapsed, collidingWith);
		break;
	case BOUNCE_HIGH:
		state = BOUNCING;
		bounceHigh(elapsed, collidingWith);
		break;
	case DESTROY:
		if (!isInvincible){
			destroy(true, true);
		}
		break;
	case DEACTIVATE:
		if (!isInvincible){
			deActivate();
		}
		break;
	case STOP:
		stop();
		break;
	case STATIC_COLLISION:
		collideWithStatic(collidingWith);
		break;
	case NOTHING:
	case WARP:
		break;
	case COLLISION_BEHAVIOR_SIZE:
		collide(elapsed, collidingWith, this->collisionBehavior);
		break;
	default:
		throw "UNRECOGNIZED COLLISION BEHAVIOR!";
		break;
	}
	postCollisionAction(sound);
}

void CompositeEntity::collideSAT(float elapsed, CompositeEntity* collidingWith, COLLISION_BEHAVIOR collisionBehavior, GameSound* sound){
	switch (collisionBehavior){
	case BOUNCE:
		state = BOUNCING;
		bounce(elapsed, collidingWith);
		break;
	case BOUNCE_HIGH:
		state = BOUNCING;
		bounceHigh(elapsed, collidingWith);
		break;
	case DESTROY:
		if (!isInvincible){
			destroy(true, true);
		}
		break;
	case DEACTIVATE:
		if (!isInvincible){
			deActivate();
		}
		break;
	case STOP:
		stop();
		break;
	case STATIC_COLLISION:
		collideWithStaticSAT(collidingWith);
		break;
	case NOTHING:
	case WARP:
		break;
	case COLLISION_BEHAVIOR_SIZE:
		collide(elapsed, collidingWith, this->collisionBehavior);
		break;
	case FALL:
		fall();
		break;
	default:
		throw "UNRECOGNIZED COLLISION BEHAVIOR!";
		break;
	}
	postCollisionAction(sound);
}

void CompositeEntity::collideWithStatic(CompositeEntity* collidingWith){
	collideLeft = (position.x - sizeNegative.x * sin(rotation) >= collidingWith->position.x - collidingWith->sizeNegative.x) && (position.y - sizeNegative.y < collidingWith->position.y);

	collideRight = position.x + sizePositive.x <= collidingWith->position.x + collidingWith->sizePositive.x && (position.y - sizeNegative.y < collidingWith->position.y);

	collideBottom = (position.y - sizeNegative.y >= collidingWith->position.y - collidingWith->sizeNegative.y) && (position.y - sizeNegative.y >= collidingWith->position.y - collidingWith->sizeNegative.y / 2);

	collideTop = (position.y + sizePositive.y <= collidingWith->position.y + collidingWith->sizePositive.y) && (position.y + sizePositive.y <= collidingWith->position.y + collidingWith->sizePositive.y / 2);

	if (collideLeft && collideRight){
		collideLeft = false;
		collideRight = false;
	}

	if (collideTop && collideBottom){
		collideBottom = false;
		collideTop = false;
	}
	float penetrationX = 0, penetrationY = 0;
	staticCollideLeft = false;
	staticCollideRight = false;
	staticCollideTop = false;
	staticCollideBottom = false;
	if (collideLeft){
		if (velocity.x < 0){
			velocity.x = 0;
			penetrationX = ((collidingWith->position.x + collidingWith->sizePositive.x) - (position.x - sizeNegative.x));
		}
		staticCollideLeft = true;
	}

	if (collideRight){
		if (velocity.x > 0){
			velocity.x = 0;
			penetrationX = -(((position.x + sizePositive.x) - (collidingWith->position.x - collidingWith->sizeNegative.x)));
		}
		staticCollideRight = true;
	}

	if (collideBottom){
		if (velocity.y < 0){
			velocity.y = 0;
			penetrationY = ((collidingWith->position.y + collidingWith->sizePositive.y) - (position.y - sizeNegative.y));
			onTileGround = true;
		}
		staticCollideBottom = true;
	}

	if (collideTop){
		if (velocity.y > 0){
			velocity.y = 0;
			penetrationY = -(((position.y + sizePositive.y) - (collidingWith->position.y - collidingWith->sizeNegative.y)));
		}
		staticCollideTop = true;
	}

	position.x += penetrationX;
	position.y += penetrationY;
	if (collidingWith->getVelocity().y > 0){
		velocity.y += collidingWith->getVelocity().y;
	}
	velocity.x += collidingWith->getVelocity().x;
}

void CompositeEntity::collideWithStaticSAT(CompositeEntity* collidingWith){
	float penetrationX = 0, penetrationY = 0;

	if ((collideRight || collideLeft || abs(acceleration.x) >= 1) && collideTop){
		collideTop = false;
	}
	if (collideLeft){
		if (velocity.x < 0){
			velocity.x = 0;
		}
		if (collidingWith->velocity.x > 0){
			velocity.x += collidingWith->velocity.x;
		}
		penetrationX = getSATPenetrationDirectional(collisionNode->getSATCoordinates(), otherCollisionNode->getSATCoordinates(), LEFT);
	}

	if (collideRight){
		velocity.x = 0;
		penetrationX = getSATPenetrationDirectional(collisionNode->getSATCoordinates(), otherCollisionNode->getSATCoordinates(), RIGHT);
	}
	
	if (collideTop){
		if (velocity.y > 0){
			velocity.y = 0;
		}
		if (collidingWith->getVelocity().y < 0){
			velocity.y -= collidingWith->getVelocity().y;
		}
		velocity.x += collidingWith->getVelocity().x;
		penetrationY = -getSATPenetrationDirectional(collisionNode->getSATCoordinates(), otherCollisionNode->getSATCoordinates(), UP);
	}

	if (collideBottom){
		if (!staticCollideBottom && velocity.y != 0){
			penetrationY = getSATPenetrationDirectional(collisionNode->getSATCoordinates(), otherCollisionNode->getSATCoordinates(), DOWN);
		}
		if (velocity.y < 0){
			velocity.y = 0;
		}
		if (collidingWith->getVelocity().y > 0){
			velocity.y += collidingWith->getVelocity().y;
		}
		velocity.x += collidingWith->getVelocity().x;
		staticCollideBottom = true;
		onTileGround = true;
		bottomStaticCollisionCount++;
	}

	if (!collideBottom){
		staticCollideBottom = false;
		onTileGround = false;
		bottomStaticCollisionCount = 0;
	}

	position.x += penetrationX;
	position.y += penetrationY;
}

void CompositeEntity::resetFlags(){
	boundaryLeft = false;
	boundaryRight = false;
	boundaryBottom = false;
	boundaryTop = false;

	collideLeft = false;
	collideRight = false;
	collideBottom = false;
	collideTop = false;

	staticCollideLeft = false;
	staticCollideRight = false;
	//staticCollideBottom = staticCollideBottom && velocity.y == 0;
	staticCollideBottom = false;
	staticCollideTop = false;
}

void CompositeEntity::transformMatrix(){
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);
	modelMatrix.Rotate(rotation);
	modelMatrix.Scale(scale.x, scale.y, scale.z);
	if (doMirror){
		modelMatrix.Yaw(M_PI);
	}
}

//Throws exception if boundary behavior unrecognized
void CompositeEntity::boundaryAction(float gameWallLeft, float gameWallRight, float gameFloor, float gameCeiling){
	switch (boundaryBehavior){
	case BOUND_BOUNCE:
		break;
	case BOUND_TURN:
		boundaryTurn(gameWallLeft, gameWallRight, gameFloor, gameCeiling);
		break;
	case BOUND_STOP:
		boundaryStop(gameWallLeft, gameWallRight, gameFloor, gameCeiling);
		break;
	case BOUND_DESTROY:
		destroy(true, false);
		break;
	case BOUND_DEACTIVATE:
		deActivate();
		break;
	//case BOUND_TURN_AND_DOWN:
	//	boundaryTurnAndDown(gameWall, gameCeiling);
	//	break;
	case BOUND_STOP_X:
		boundaryStopAtWall(gameWallLeft, gameWallRight);
		break;
	case BOUND_NOTHING:
		break;
	case BOUND_DESTROY_NO_ANIM:
		destroy(false, false);
		break;
	case BOUND_DEACTIVATE_UNDER_FLOOR:
		boundaryStopAtWall(gameWallLeft, gameWallRight);
		if (position.y - abs(sizePositive.y * cos(rotation) - sizeNegative.x * sin(rotation)) <= gameFloor){
			deActivate();
		}
		break;
	default:
		throw "UNKNOWN BOUNDARY BEHAVIOR!";
		break;
	}
}

void CompositeEntity::blink(){
	if (first != nullptr){
		first->blinkAll();
	}
}

void CompositeEntity::setAllRender(bool doRender){
	if (first != nullptr){
		first->setAllDoRender(doRender);
	}
}

CompositeEntity* CompositeEntity::fire(){
	if (timeSinceFiring >= firingDelay && isActive && projectile != nullptr){
		CompositeEntity* newProjectile = new CompositeEntity();
		newProjectile->deepCopy(projectile);
		newProjectile->setVelocity(3 * (doMirror ? -topSpeed : topSpeed) * cos(rotation), 3 * (doMirror ? -topSpeed : topSpeed) * sin(rotation));
		if (type == HIDING_ENEMY_FIRE || type == ENEMY_TURRET){
			newProjectile->setVelocity((doMirror ? 100 : -100), 0, 0);
		}
		newProjectile->setPosition(position.x + sin(rotation), position.y + cos(rotation));
		newProjectile->setRotation(rotation);
		newProjectile->setIsActive(true);
		newProjectile->setObjectLayer(objectLayer);
		newProjectile->setRenderLayer(renderLayer);
		newProjectile->setState(MOVING);
		newProjectile->setCanCollideWithTiles(true);

		timeSinceFiring = 0;
		playTriggerSound(TRIG_FIRE);
		first->startAnimation(ANIMATION_FIRE);
		firing = true;
		
		
		return newProjectile;
	}
	return nullptr;
}

void CompositeEntity::addToTimeSinceFiring(float elapsed){
	timeSinceFiring += elapsed;
}

CompositeEntity* CompositeEntity::logic(CompositeEntity* player, CompositeEntity* lastProjectile){
	switch (type){
	case ACTOR_ENEMY:
		if ((player->getPosition().x - player->getSizeNegative().x <= position.x + sizePositive.x && player->getPosition().x + player->getSizePositive().x >= position.x - sizeNegative.x) && isActive){
			if (lastProjectile == nullptr){
				return fire();
			}
			else{
				if (abs((abs(lastProjectile->getPosition().y) - abs(position.y))) >= 1.5){
					return fire();
				}
			}
		}
		return nullptr;
		break;
	case HIDING_ENEMY_FIRE:
		if (abs(player->getPosition().x - position.x) >= awarenessRadius){
			doMirror = player->getPosition().x > position.x;
			if (!firing || first->animationComplete(ANIMATION_FIRE)){
				first->startAnimation(ANIMATION_IDLE);
			}
			return nullptr;
		}
		else if (abs(player->getPosition().x - position.x) < awarenessRadius && abs(player->getPosition().x - position.x) >= hidingRadius){
			doMirror = player->getPosition().x > position.x;
			if (abs(player->getPosition().y - player->getSizeNegative().y) >= abs(position.y - sizeNegative.y) && abs(player->getPosition().y + player->getSizePositive().y) <= abs(position.y)){
				CompositeEntity* projectile = fire();
				if (projectile == nullptr && (!firing || first->animationComplete(ANIMATION_FIRE) || first->getCurrentAnimation() != ANIMATION_FIRE)){
					first->startAnimation(ANIMATION_IDLE);
				}
				return projectile;
			}
			else{
				first->startAnimation(ANIMATION_IDLE);
				return nullptr;
			}
		}
		else if (abs(player->getPosition().x - position.x) < hidingRadius){
			doMirror = player->getPosition().x > position.x;
			if (!firing || first->animationComplete(ANIMATION_FIRE) || first->getCurrentAnimation() != ANIMATION_FIRE){
				first->startAnimation(ANIMATION_HIDING);
			}
			return nullptr;
		}
		else{
			if (!firing || first->animationComplete(ANIMATION_FIRE) || first->getCurrentAnimation() != ANIMATION_FIRE){
				first->startAnimation(ANIMATION_HIDING);
			}
			return nullptr;
		}
		return nullptr;
		break;
	case ENEMY_TURRET:
		if (abs(player->getPosition().x - position.x) >= awarenessRadius){
			doMirror = player->getPosition().x > position.x;
			first->startAnimation(ANIMATION_IDLE);
			return nullptr;
		}
		else if (abs(player->getPosition().x - position.x) < awarenessRadius){
			doMirror = player->getPosition().x > position.x;
			if (abs(player->getPosition().y - player->getSizeNegative().y) <= abs(position.y - sizeNegative.y)){
				CompositeEntity* projectile = fire();
				if (projectile == nullptr){
					first->startAnimation(ANIMATION_IDLE);
				}
				return projectile;
			}
			else{
				return nullptr;
			}
		}
		else{
			first->startAnimation(ANIMATION_HIDING);
			return nullptr;
		}
		break;
	default:
		return nullptr;
		break;
	}
}

void CompositeEntity::setAwarenessRadius(int radius){
	awarenessRadius = radius;
}

void CompositeEntity::setHidingRadius(int radius){
	hidingRadius = radius;
}

void CompositeEntity::setDisplayText(const std::string& text){
	this->text = text;
}


const std::string& CompositeEntity::getDisplayText(){
	return text;
}

void CompositeEntity::setTextSheet(GLuint textSheet){
	this->textSheet = textSheet;
}

void CompositeEntity::setJumpSpeed(float jumpSpeed){
	this->jumpSpeed = jumpSpeed;
}

void CompositeEntity::changeAnimation(ANIMATION_TYPE animationType){
	if (first != nullptr && isActive){
		first->startAnimation(animationType);
	}
}

void CompositeEntity::runAnimation(float elapsed, float fps){
	if (first != nullptr && isActive){
		first->runAnimation(elapsed, fps);
		if (first->animationComplete(ANIMATION_FIRE)){
			firing = false;
			if (first->hasAnimation(ANIMATION_FIRE_RETURN)){
				first->startAnimation(ANIMATION_FIRE_RETURN);
			}
			else{
				if (first->hasAnimation(ANIMATION_IDLE)){
					first->startAnimation(ANIMATION_IDLE);
				}
			}
		}
		if (first->animationComplete(ANIMATION_FIRE_RETURN)){
			if (first->hasAnimation(ANIMATION_IDLE)){
				first->startAnimation(ANIMATION_IDLE);
			}
		}
	}
}

void CompositeEntity::setCanCollideWithTiles(bool canCollide){
	canCollideWithTiles = canCollide;
}

bool CompositeEntity::getCanCollideWithTiles(){
	return canCollideWithTiles;
}

void CompositeEntity::deepCopy(CompositeEntity* toCopy){
	if (toCopy->first != nullptr){
		first = new Entity();
		first->deepCopy(toCopy->first);
	}
	if (toCopy->projectile != nullptr){
		this->projectile = new CompositeEntity();
		this->projectile->deepCopy(toCopy->projectile);
	}
	if (toCopy->particleEmitters.size() != 0){
		if (toCopy->particleEmitters.find(CONSTANT_EMITTER) != toCopy->particleEmitters.end()){
			for (size_t i = 0; i < toCopy->particleEmitters[CONSTANT_EMITTER].size(); i++){
				ParticleEmitter* tmp = new ParticleEmitter;
				tmp->deepCopy(toCopy->particleEmitters[CONSTANT_EMITTER][i]);
				particleEmitters[CONSTANT_EMITTER].push_back(tmp);
			}
		}
		if (toCopy->particleEmitters.find(COLLISION_EMITTER) != toCopy->particleEmitters.end()){
			for (size_t i = 0; i < toCopy->particleEmitters[COLLISION_EMITTER].size(); i++){
				ParticleEmitter* tmp = new ParticleEmitter;
				tmp->deepCopy(toCopy->particleEmitters[COLLISION_EMITTER][i]);
				particleEmitters[COLLISION_EMITTER].push_back(tmp);
			}
		}
		if (toCopy->particleEmitters.find(HARD_COLLISION_EMITTER) != toCopy->particleEmitters.end()){
			for (size_t i = 0; i < toCopy->particleEmitters[HARD_COLLISION_EMITTER].size(); i++){
				ParticleEmitter* tmp = new ParticleEmitter;
				tmp->deepCopy(toCopy->particleEmitters[HARD_COLLISION_EMITTER][i]);
				particleEmitters[HARD_COLLISION_EMITTER].push_back(tmp);
			}
		}
	}

	if (toCopy->collisionSounds.size() != 0){
		for (std::unordered_map<unsigned, std::unordered_map<unsigned, GameSound*>>::iterator itr = toCopy->collisionSounds.begin(); itr != toCopy->collisionSounds.end(); itr++){
			if (itr->second.size() != 0){
				for (std::unordered_map<unsigned, GameSound*>::iterator soundItr = itr->second.begin(); soundItr != itr->second.end(); soundItr++){
					addCollisionSound(soundItr->second);
				}
			}
		}
	}

	if (toCopy->triggerSounds.size() != 0){
		for (std::unordered_map<unsigned, GameSound*>::iterator itr = toCopy->triggerSounds.begin(); itr != toCopy->triggerSounds.end(); itr++){
			addTriggerSound(itr->second);
		}
	}
	this->startingPosition = toCopy->startingPosition;
	this->startingVelocity = toCopy->startingVelocity;
	this->startingAcceleration = toCopy->startingAcceleration;
	this->sizePositive = toCopy->sizePositive;
	this->sizeNegative = toCopy->sizeNegative;
	this->startingScale = toCopy->startingScale;

	this->renderLayer = toCopy->renderLayer;
	this->objectLayer = toCopy->objectLayer;

	this->startingRotation = toCopy->startingRotation;
	this->startingRotationalVelocity = toCopy->startingRotationalVelocity;
	this->topSpeed = toCopy->topSpeed;
	this->firingDelay = toCopy->firingDelay;

	this->type = toCopy->type;
	this->boundingType = toCopy->boundingType;
	this->collisionBehavior = toCopy->collisionBehavior;
	this->boundaryBehavior = toCopy->boundaryBehavior;
	this->tileCollisionBehavior = toCopy->tileCollisionBehavior;
	this->tileLogicBehavior = toCopy->tileLogicBehavior;
	this->state = toCopy->state;

	this->isActive = toCopy->isActive;
	this->canCollide = toCopy->canCollide;
	this->falls = toCopy->falls;
	this->isInvincible = toCopy->isInvincible;

	this->text = toCopy->text;
	this->textSheet = toCopy->textSheet;
	this->size = toCopy->size;
	this->spacing = toCopy->spacing;
	this->position = toCopy->position;
	this->scale = toCopy->scale;
	this->entityID = toCopy->entityID;
	this->jumpSpeed = toCopy->jumpSpeed;
	this->awarenessRadius = toCopy->awarenessRadius;
	this->hidingRadius = toCopy->hidingRadius;
	this->canCollideWithTiles = toCopy->canCollideWithTiles;

	centralized = toCopy->centralized;

	bottomStaticCollisionCount = 0;
}

Matrix& CompositeEntity::getMatrix(){
	return modelMatrix;
}

bool CompositeEntity::getIsStatic(){
	return isStatic;
}


void CompositeEntity::setTileCollisionBehavior(TILE_COLLISION_BEHAVIOR behavior){
	tileCollisionBehavior = behavior;
}

void CompositeEntity::setIsStatic(bool isStatic){
	this->isStatic = isStatic;
}

TILE_COLLISION_BEHAVIOR CompositeEntity::getTileCollisionBehavior(){
	return tileCollisionBehavior;
}

void CompositeEntity::bounceHigh(float elapsed, CompositeEntity* bouncingOffOf){
	velocity.y = jumpSpeed + 5;
	setState(JUMPING);
	playTriggerSound(TRIG_BOUNCE_HIGH);
}

void CompositeEntity::checkPoint(){
	checkPointPosition = position;
}

void CompositeEntity::resetToCheckpoint(){
	position = checkPointPosition;
}

void CompositeEntity::setWarpDestination(const std::string& warpDestination){
	this->warpLevelDestination = warpDestination;
}

const std::string& CompositeEntity::getWarpDestination(){
	return warpLevelDestination;
}

void CompositeEntity::addParticleEmitter(ParticleEmitter* emitter){
	if (emitter == nullptr){
		throw "Error: tried to add nullptr emitter!";
	}
	particleEmitters[emitter->emitterType].push_back(emitter);
}

void CompositeEntity::hardCollision(DIRECTION collisionDirection, int particleCount){
	if (particleEmitters.size() != 0 && particleEmitters[HARD_COLLISION_EMITTER].size() != 0){
		for (size_t i = 0; i < particleEmitters[HARD_COLLISION_EMITTER].size(); i++){
			particleEmitters[HARD_COLLISION_EMITTER][i]->trigger(particleCount);
		}
	}
	playCollisionSound(collisionDirection, STRONG);
}

void CompositeEntity::addCollisionSound(GameSound* gameSound){
	if (gameSound->getSoundType() != COLLISION_SOUND){
		throw "Incorrect sound type!";
	}
	DIRECTION direction = gameSound->getCollisionDirection();
	COLLISION_STRENGTH collisionStrength = gameSound->getCollisionStrength();
	if (direction == DIRECTION_SIZE){
		throw "Invalid direction!";
	}
	if (collisionStrength == COLLISION_STRENGTH_SIZE){
		throw "Invlaid collision strength!";
	}
	if (direction == ALL_DIRECTIONS){
		if (collisionSounds.find(collisionStrength) != collisionSounds.end() && collisionSounds[collisionStrength].size() != 0){
			throw "Entity already has sounds associated with at least one direction!";
		}
		collisionSounds[collisionStrength][UP] = gameSound;
		collisionSounds[collisionStrength][DOWN] = gameSound;
		collisionSounds[collisionStrength][LEFT] = gameSound;
		collisionSounds[collisionStrength][RIGHT] = gameSound;
	}
	else if (direction == X){
		if (collisionSounds.find(collisionStrength) != collisionSounds.end() && (collisionSounds[collisionStrength].find(LEFT) != collisionSounds[collisionStrength].end() || collisionSounds[collisionStrength].find(RIGHT) != collisionSounds[collisionStrength].end())){
			throw "Entity already has collision sounds in X direction for this strength!";
		}
		collisionSounds[collisionStrength][LEFT] = gameSound;
		collisionSounds[collisionStrength][RIGHT] = gameSound;
	}
	else if (direction == Y){
		if (collisionSounds.find(collisionStrength) != collisionSounds.end() && (collisionSounds[collisionStrength].find(DOWN) != collisionSounds[collisionStrength].end() || collisionSounds[collisionStrength].find(UP) != collisionSounds[collisionStrength].end())){
			throw "Entity already has collision sounds in X direction for this strength!";
		}
		collisionSounds[collisionStrength][DOWN] = gameSound;
		collisionSounds[collisionStrength][UP] = gameSound;
	}
	else{
		if (collisionSounds.find(collisionStrength) != collisionSounds.end() && collisionSounds[collisionStrength].find(direction) != collisionSounds[collisionStrength].end()){
			throw "Trying to add an already existing sound!";
		}
		collisionSounds[collisionStrength][direction] = gameSound;
	}
}

void CompositeEntity::addTriggerSound(GameSound* gameSound){
	if (gameSound->getSoundType() != TRIGGER_SOUND){
		throw "Incorrect sound type!";
	}
	SOUND_TRIGGER soundTrigger = gameSound->getSoundTrigger();
	if (triggerSounds.find(soundTrigger) != triggerSounds.end()){
		throw "Trying to add already existing sound!";
	}
	triggerSounds[soundTrigger] = gameSound;
}

float CompositeEntity::getJumpSpeed(){
	return jumpSpeed;
}

void CompositeEntity::freeMemory(){
	if (first != nullptr){
		try{
			first->freeMemory();
			delete first;
			first = nullptr;
		}
		catch (char* e){
			first = nullptr;
		}
	}

	if (particleEmitters.size() != 0){
		for (std::unordered_map<unsigned, std::vector<ParticleEmitter*>>::iterator itr = particleEmitters.begin(); itr != particleEmitters.end(); itr++){
			if (itr->second.size() != 0){
				for (std::vector<ParticleEmitter*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.begin(); itr++){
					if (*itr2 != nullptr){
						try{
							(*itr2)->freeMemory();
							delete *itr2;
							*itr2 = nullptr;
						}
						catch (char* e){
							*itr2 = nullptr;
						}
					}
					itr2 = itr->second.erase(itr2);
					if (itr2 == itr->second.end()){
						break;
					}
				}
			}
			itr = particleEmitters.erase(itr);
			if (itr == particleEmitters.end()){
				break;
			}
		}
	}

	if (collisionSounds.size() != 0){
		for (std::unordered_map<unsigned, std::unordered_map<unsigned, GameSound*>>::iterator itr = collisionSounds.begin(); itr != collisionSounds.end(); itr++){
			if (itr->second.size() != 0){
				for (std::unordered_map<unsigned, GameSound*>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
					if (itr2->second != nullptr){
						itr2->second = nullptr;
					}
					itr2 = itr->second.erase(itr2);
					if (itr2 == itr->second.end()){
						break;
					}
				}
			}
			itr = collisionSounds.erase(itr);
			if (itr == collisionSounds.end()){
				break;
			}
		}
	}

	if (triggerSounds.size() != 0){
		for (std::unordered_map<unsigned, GameSound*>::iterator itr = triggerSounds.begin(); itr != triggerSounds.end(); itr++){
			if (itr->second != nullptr){
				itr->second = nullptr;
			}
			itr = triggerSounds.erase(itr);
			if (itr == triggerSounds.end()){
				break;
			}
		}
	}

	if (projectile != nullptr){
		try{
			projectile->freeMemory();
			delete projectile;
			projectile = nullptr;
		}
		catch (char* e){
			projectile = nullptr;
		}
	}
}

void CompositeEntity::projectToAxis(){
	Vector3 uR, uL, lL, lR;
	Vector3 uR_, uL_, lL_, lR_;

	uR_.x = sizePositive.x;
	uR_.y = sizePositive.y;

	uL_.x = -sizePositive.x;
	uL_.y = sizePositive.y;

	lL_.x = -sizePositive.x;
	lL_.y = -sizeNegative.y;

	lR_.x = sizePositive.x;
	lR_.y = -sizePositive.y;

	uR.x = uR_.x * cos(rotation) - uR_.y * sin(rotation) + position.x;
	uR.y = uR_.x * sin(rotation) + uR_.y * cos(rotation) + position.y;
	uR.z = 1;

	uL.x = uL_.x * cos(rotation) - uL_.y * sin(rotation) + position.x;
	uL.y = uL_.x * sin(rotation) + uL_.y * cos(rotation) + position.y;
	uL.z = 1;

	lR.x = lR_.x * cos(rotation) - lR_.y * sin(rotation) + position.x;
	lR.y = lR_.x * sin(rotation) + lR_.y * cos(rotation) + position.y;
	lR.z = 1;

	lL.x = lL_.x * cos(rotation) - lL_.y * sin(rotation) + position.x;
	lL.y = lL_.x * sin(rotation) + lL_.y * cos(rotation) + position.y;
	lL.z = 1;

	SATcoordinates = { uR, uL, lL, lR };
}

const std::vector<Vector3>& CompositeEntity::getSATCoordinates(){
	return SATcoordinates;
}