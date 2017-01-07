#ifndef _PARTICLE_H
#define _PARTICLE_H
#include "Color.h"
#include "Vector3.h"
#include "Texture.h"

struct Particle
{
	Vector3 position;
	Vector3 velocity;
	float sizeOffset;
	float timeAlive;
	Color colorDeviation;
	bool isActive;

	inline void update(float elapsed, float gravity){
		velocity.y += gravity * elapsed;

		position.x += velocity.x * elapsed;
		position.y += velocity.y * elapsed;
		position.z += velocity.z * elapsed;

		timeAlive += elapsed;
	}
};

#endif