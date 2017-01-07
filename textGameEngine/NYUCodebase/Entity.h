#ifndef ENTITY_H
#define ENTITY_H
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include "Vector3.h"
#include "Matrix.h"
#include <vector>
#include "Animation.h"
#include <unordered_map>
#include <string>

enum BOUNDING_TYPE{ SQUARE, CIRCLE };

class Texture;
class ShaderProgram;
class Entity
{
protected:
	Texture* texture;
	Entity* child;
	Entity* sibling;

	Vector3 position;
	Vector3 offsetPosition;
	Vector3 velocity;
	Vector3 acceleration;

	Vector3 size;
	Vector3 scale;
	Vector3 offsetScale;
	Vector3 baseSize;

	float rotation;
	float offsetRotation;
	float rotationalVelocity;

	bool canCollide;
	bool doRender;
	bool isAnimated;
	bool doMirror;
	BOUNDING_TYPE boundingType;
	//std::unordered_map<unsigned, Animation*> animations;
	std::vector<Vector3> SATCoordinates;
	void projectToAxis(Vector3 position, Vector3 offsetScale, float rotation);
	ANIMATION_TYPE currentAnimation;
	std::string entityID;

	Matrix modelMatrix;
	Matrix offsetModelMatrix;
	void transformMatrix(Matrix offset);

	void transformMatrix(Vector3 positionOffset, Vector3 scaleOffset, float rotationOffset);

	std::vector<GLfloat> objectVertices;
	std::vector<GLfloat> textureCoordinates;
public:
	Entity();
	Entity(const std::string& entityID, Texture* texture);
	~Entity();

	std::unordered_map<unsigned, Animation*> animations;


	const std::string& getEntityID();
	Vector3 getPosition();
	Vector3 getVelocity();
	Vector3 getAcceleration();
	Vector3 getSize();
	Vector3 getScale();
	float getRotation();
	float getRotationalVelocity();
	Entity* getChild();
	Entity* getSibling();
	BOUNDING_TYPE getBoundingType();
	bool getCanCollide();
	bool getDoRender();
	bool getIsAnimated();
	bool getDoMirror();
	bool hasAnimation(ANIMATION_TYPE animationType);
	bool animationComplete(ANIMATION_TYPE animationType);

	void setEntityID(const std::string& entityID);
	void setTexture(Texture* texture);
	void setPosition(float x, float y, float z = 0.0f);
	void setVelocity(float x, float y, float z = 0.0f);
	void setAcceleration(float x, float y, float z = 0.0f);
	void setSize(float x, float y, float z = 1.0f);
	void setScale(float x, float y, float z = 1.0f);
	void setRotation(float rotation);
	void setRotationalVelocity(float rotationalVelocity);
	void setChild(Entity* next);
	void setSibling(Entity* sibling);
	void setObjectVertices(const std::vector<GLfloat>& objectVertices);
	void setBoundingType(BOUNDING_TYPE boundingType);
	void setCanCollide(bool canCollide);
	void setDoRender(bool doRender);
	void setDoMirror(bool doMirror);
	void setIsAnimated(bool isAnimated);
	void addAnimation(Animation* animation);

	void freeMemory();
	void move(float elapsed);
	void addSibling(Entity* siblingToAdd);
	void draw(ShaderProgram* program, Matrix offset, float elapsed, float fps);

	void draw(ShaderProgram* program, Vector3 positionOffset, Vector3 scaleOffset, float rotationOffset, float elapsed, float fps);

	void blink();
	void blinkAll();
	void setAllDoRender(bool doRender);

	void updateBounding(Vector3 scale, Vector3 position, float rotation);
	
	const std::vector<Vector3>& getSATCoordinates();

	void startAnimation(ANIMATION_TYPE animation);

	void runAnimation(float elapsed, float fps);

	virtual void deepCopy(Entity* toCopy);

	void centralize(Vector3 offset);

	int getNumOfEntities(bool lookForCollision = false);
	ANIMATION_TYPE getCurrentAnimation();
};

#endif