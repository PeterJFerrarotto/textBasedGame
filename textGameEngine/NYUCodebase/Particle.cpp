#include "Particle.h"


Particle::Particle()
{
}

void Particle::update(float elapsed){
	position.x += velocity.x * elapsed;
	position.y += velocity.y * elapsed;
	position.z += velocity.z * elapsed;
	timeAlive += elapsed;
}

Particle::~Particle()
{
}
