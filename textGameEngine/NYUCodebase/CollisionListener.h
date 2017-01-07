#ifndef COLLISION_LISTENER_H
#define COLLISION_LISTENER_H
#include <unordered_map>
#include "EnumClassHash.h"
#include "CompositeEntity.h"
#include <vector>

class CollisionListener
{
protected:
	unsigned entityType1, entityType2;

	COLLISION_BEHAVIOR behaviorOfEntity1, behaviorOfEntity2;

	GameSound* soundToPlay;
	
	bool up, down, left, right;

	void collide(float elapsed, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities);
public:
	CollisionListener(unsigned entityType1, unsigned entityType2, COLLISION_BEHAVIOR behaviorOfEntity1 = COLLISION_BEHAVIOR_SIZE, COLLISION_BEHAVIOR behaviorOfEntity2 = COLLISION_BEHAVIOR_SIZE, bool entity2Down = true, bool entity2Up = true, bool entity2Left = true, bool entity2Right = true, GameSound* sound = nullptr);
	~CollisionListener();

	void operator()(float elapsed, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities);
	
	void freeMemory();
};

#endif