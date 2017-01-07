#include "ParticleEmitter.h"
#include "mathHelper.h"

ParticleEmitter::ParticleEmitter()
{
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::update(float elapsed, float gravity, Vector3 parentVelocity, Vector3 parentPosition){
	offsetPosition.x = position.x + parentPosition.x;
	offsetPosition.y = position.y + parentPosition.y;
	offsetPosition.z = position.z + parentPosition.z;

	offsetVelocity.x = velocity.x + parentVelocity.x;
	offsetVelocity.y = velocity.y + parentVelocity.y;
	offsetVelocity.z = velocity.z + parentVelocity.z;
	for (size_t i = 0; i < particles.size(); i++){
		if (particles[i] != nullptr && particles[i]->isActive){
			if (particles[i]->timeAlive >= maxLifeTime){
				particles[i]->position = offsetPosition;
				particles[i]->velocity.x = randomRanged(velocityDeviation.x);
				particles[i]->velocity.y = randomRanged(velocityDeviation.y);
				particles[i]->velocity.z = randomRanged(velocityDeviation.z);

				particles[i]->timeAlive = randomRanged(maxLifeTime);
				particles[i]->sizeOffset = randomRanged(sizeDeviation);

				particles[i]->colorDeviation.r = randomRanged(colorDeviation.r);
				particles[i]->colorDeviation.b = randomRanged(colorDeviation.b);
				particles[i]->colorDeviation.g = randomRanged(colorDeviation.g);
				particles[i]->colorDeviation.a = randomRanged(colorDeviation.a);
				
				particles[i]->isActive = emitterType == CONSTANT_EMITTER;
			}
			else{
				particles[i]->update(elapsed, gravity);
			}
		}
	}
}

void ParticleEmitter::render(ShaderProgram* program){
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<float> colors;
	if (textured){
		for (size_t i = 0; i < particles.size(); i++){
			if (particles[i] != nullptr && particles[i]->isActive){
				float m = particles[i]->timeAlive / maxLifeTime;
				float size = lerp(startSize, endSize, m) + particles[i]->sizeOffset;

				vertices.insert(vertices.end(), { particles[i]->position.x - size, particles[i]->position.y + size, particles[i]->position.x - size, particles[i]->position.y - size, particles[i]->position.x + size, particles[i]->position.y + size,
					particles[i]->position.x + size, particles[i]->position.y + size, particles[i]->position.x - size, particles[i]->position.y - size, particles[i]->position.x + size, particles[i]->position.y - size
				});
				texCoords.insert(texCoords.end(), { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
				});

				for (int j = 0; j < 6; j++) {
					colors.push_back(lerp(startColor.r, endColor.r, m) + particles[i]->colorDeviation.r);
					colors.push_back(lerp(startColor.g, endColor.g, m) + particles[i]->colorDeviation.g);
					colors.push_back(lerp(startColor.b, endColor.b, m) + particles[i]->colorDeviation.b);
					colors.push_back(lerp(startColor.a, endColor.a, m) + particles[i]->colorDeviation.a);
				}
			}
		}
		if (vertices.size() != 0){
			Matrix matrix;
			glBindTexture(GL_TEXTURE_2D, particleTexture->getTextureID());
			program->setModelMatrix(matrix);

			glEnableVertexAttribArray(program->positionAttribute);
			glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());

			glEnableVertexAttribArray(program->texCoordAttribute);
			glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());

			GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
			glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, colors.data());
			glEnableVertexAttribArray(colorAttribute);

			glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 2);
			glDisableVertexAttribArray(program->positionAttribute);
			glDisableVertexAttribArray(program->texCoordAttribute);
			glDisableVertexAttribArray(colorAttribute);
		}
	}
	else{
		for (size_t i = 0; i < particles.size(); i++){
			if (particles[i] != nullptr && particles[i]->isActive){
				float m = particles[i]->timeAlive / maxLifeTime;

				vertices.insert(vertices.end(), { particles[i]->position.x, particles[i]->position.y });
				colors.push_back(lerp(startColor.r, endColor.r, m) + particles[i]->colorDeviation.r);
				colors.push_back(lerp(startColor.g, endColor.g, m) + particles[i]->colorDeviation.g);
				colors.push_back(lerp(startColor.b, endColor.b, m) + particles[i]->colorDeviation.b);
				colors.push_back(lerp(startColor.a, endColor.a, m) + particles[i]->colorDeviation.a);
			}
		}
		if (vertices.size() != 0){
			glEnableVertexAttribArray(program->positionAttribute);
			glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data()); 
			
			GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
			glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, colors.data());
			glEnableVertexAttribArray(colorAttribute);

			glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
			glDisableVertexAttribArray(program->positionAttribute);
			glDisableVertexAttribArray(colorAttribute);
		}
	}
}

void ParticleEmitter::trigger(int toEmit){
	if (emitterType == CONSTANT_EMITTER){
		throw "Tried to trigger constant emitter!";
	}
	if (toEmit == 0){
		throw "Tried to emit zero particles!";
	}
	int activated = 0;
	for (size_t i = 0; i < particles.size(); i++){
		if (activated == toEmit){
			break;
		}
		if (particles[i]->isActive == false){
			particles[i]->position = offsetPosition;
			particles[i]->position.x += position.x;
			particles[i]->position.y += position.y;
			particles[i]->position.z += position.z;
			particles[i]->velocity.x = randomRanged(velocityDeviation.x);
			particles[i]->velocity.y = randomRanged(velocityDeviation.y);
			particles[i]->velocity.z = randomRanged(velocityDeviation.z);

			particles[i]->timeAlive = randomRanged(maxLifeTime);
			particles[i]->sizeOffset = randomRanged(sizeDeviation);

			particles[i]->colorDeviation.r = randomRanged(colorDeviation.r);
			particles[i]->colorDeviation.b = randomRanged(colorDeviation.b);
			particles[i]->colorDeviation.g = randomRanged(colorDeviation.g);
			particles[i]->colorDeviation.a = randomRanged(colorDeviation.a);
			particles[i]->isActive = true;
			activated++;
		}
	}
}

void ParticleEmitter::deepCopy(ParticleEmitter* toCopy){
	maxLifeTime = toCopy->maxLifeTime;
	startSize = toCopy->startSize;
	endSize = toCopy->endSize;
	sizeDeviation = abs(endSize - startSize);
	maximumParticleCount = toCopy->maximumParticleCount;
	textured = toCopy->textured;
	particleTexture = toCopy->particleTexture;
	emitterType = toCopy->emitterType;
	position = toCopy->position;
	velocity = toCopy->velocity;
	velocityDeviation = toCopy->velocityDeviation;
	startColor = toCopy->startColor;
	endColor = toCopy->endColor;
	offsetPosition = toCopy->offsetPosition;
	colorDeviation.r = abs(endColor.r - startColor.r);
	colorDeviation.b = abs(endColor.b - startColor.b);
	colorDeviation.g = abs(endColor.g - startColor.g);
	colorDeviation.a = abs(endColor.a - startColor.a);

	for (size_t i = 0; i <= maximumParticleCount; i++){
		Particle* tmp = new Particle;
		tmp->position = offsetPosition;
		tmp->velocity.x = randomRanged(velocityDeviation.x);
		tmp->velocity.y = randomRanged(velocityDeviation.y);
		tmp->velocity.z = randomRanged(velocityDeviation.z);

		tmp->timeAlive = randomRanged(maxLifeTime);
		tmp->sizeOffset = randomRanged(sizeDeviation);

		tmp->colorDeviation.r = randomRanged(colorDeviation.r);
		tmp->colorDeviation.b = randomRanged(colorDeviation.b);
		tmp->colorDeviation.g = randomRanged(colorDeviation.g);
		tmp->colorDeviation.a = randomRanged(colorDeviation.a);
		tmp->isActive = emitterType == CONSTANT_EMITTER;
		particles.push_back(tmp);
	}
}

void ParticleEmitter::freeMemory(){
	if (particles.size() != 0){
		for (std::vector<Particle*>::iterator itr = particles.begin(); itr != particles.end(); itr++){
			if (*itr != nullptr){
				try{
					(*itr)->isActive;
					delete *itr;
					*itr = nullptr;
				}
				catch (char* e){
					*itr = nullptr;
				}
			}
			itr = particles.erase(itr);
			if (itr == particles.end()){
				break;
			}
		}
	}
	if (particleTexture != nullptr){
		try{
			particleTexture->getObjectCoordinates();
			delete particleTexture;
			particleTexture = nullptr;
		}
		catch (char* e){
			particleTexture = nullptr;
		}
	}
}