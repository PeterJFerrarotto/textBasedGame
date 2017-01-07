#ifndef _PARTICLE_EMITTER_H
#define _PARTICLE_EMITTER_H
#include <vector>
#include "Color.h"
#include "Vector3.h"
#include "Particle.h"
#include "ShaderProgram.h"

enum EMITTER_TYPE {CONSTANT_EMITTER, COLLISION_EMITTER, HARD_COLLISION_EMITTER};

class ParticleEmitter
{
public:
	float maxLifeTime;
	float startSize, endSize, sizeDeviation;
	int maximumParticleCount;
	Texture* particleTexture;
	bool textured;
	
	Vector3 position;
	Vector3 offsetPosition;
	Vector3 velocity;
	Vector3 offsetVelocity;
	Vector3 velocityDeviation;

	EMITTER_TYPE emitterType;
	std::vector<Particle*> particles;
	Color startColor, endColor, colorDeviation;

	ParticleEmitter();
	~ParticleEmitter();
	void deepCopy(ParticleEmitter* toCopy);
	void render(ShaderProgram* program);
	void update(float elapsed, float gravity, Vector3 parentVelocity, Vector3 parentPosition);
	void trigger(int toEmit = 1);

	void freeMemory();
};

#endif