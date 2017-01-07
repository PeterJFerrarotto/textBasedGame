#ifndef XML_HELPER_H
#define XML_HELPER_H
#include "GameEngine.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "dirent.h"
#include "CompositeEntity.h"
#include "Entity.h"
#include "Texture.h"
#include "Animation.h"
#include "Level.h"
#include "ParticleEmitter.h"
#include "SDL_mixer.h"
#include <string>
#include <unordered_map>
#include <sstream>
using namespace std;
using namespace rapidxml;

//#define _CRT_SECURE_NO_WARNINGS
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

static unordered_map<unsigned, CompositeEntity*> entityTypes;
static unordered_map<GLuint, pair<int, int>> loadedTextureSizes;

static const char* txtExt = ".txt";
static const char* xmlExt = ".xml";
static const char* tsxExt = ".tsx";
static const char* pngExt = ".png";
static const char* separator = "/";

inline GLuint loadTexture(const char* imagePath){
	static unordered_map<string, GLuint> loadedTextures;
	if (loadedTextures.find(imagePath) == loadedTextures.end()){
		SDL_Surface *surface = IMG_Load(imagePath);
		GLuint textureID;
		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);

		GLenum externalFormat = GL_RGBA;
		if (surface->format->BytesPerPixel == 3) {
			externalFormat = GL_RGB;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, externalFormat, GL_UNSIGNED_BYTE, surface->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		char* x = new char[360];
		strcpy(x, imagePath);
		loadedTextures[x] = textureID;
		loadedTextureSizes[textureID] = { surface->w, surface->h };
		return textureID;
	}
	return loadedTextures.at(imagePath);
}


inline xml_document<> * loadXMLFile(const char* filePath){
	file<>* xmlFile = new file<>(filePath);
	xml_document<>* doc = new xml_document<>;
	doc->parse<0>(xmlFile->data());
	return doc;
}

inline Texture* enrichTextureInformation(xml_node<>* textureNode){
	if (textureNode == nullptr){
		return nullptr;
	}
	static unordered_map<string, pair<GLuint, xml_document<>*>> loadedFiles;
	static unordered_map<string, GLuint> loadedFilesWithoutXML;
	string directory = RESOURCE_FOLDER;
	GLuint textureID;
	xml_node<>* detailsNode;
	TEXTURE_TYPE textureType = static_cast<TEXTURE_TYPE>(stoi(textureNode->first_attribute("type")->value()));
	if (textureType == IMAGE){
		detailsNode = textureNode->first_node("Image");
		directory = directory + detailsNode->first_attribute("Directory")->value();
		string imageName = detailsNode->first_attribute("imageName")->value();
		string imagePath = directory + imageName;
		int layer = stoi(textureNode->first_attribute("layer")->value());

		if (loadedFilesWithoutXML.find(imagePath) == loadedFilesWithoutXML.end()){
			textureID = loadTexture(imagePath.c_str());
			loadedFilesWithoutXML[imagePath] = textureID;
		}
		else{
			textureID = loadedFilesWithoutXML[imagePath];
		}
		Texture* tex = new Texture(textureID, layer);
		return tex;
	}
	else if (textureType == EVEN_SPRITESHEET){
		detailsNode = textureNode->first_node("EvenSpritesheet");
		directory = directory + detailsNode->first_attribute("Directory")->value();
		string sheetName = detailsNode->first_attribute("sheetName")->value();
		string sheetPath = directory + sheetName;
		int layer = stoi(textureNode->first_attribute("layer")->value());
		unsigned index = stoi(detailsNode->first_attribute("index")->value());
		unsigned spriteCountX = stoi(detailsNode->first_attribute("spriteCountX")->value());
		unsigned spriteCountY = stoi(detailsNode->first_attribute("spriteCountY")->value());
		if (loadedFilesWithoutXML.find(sheetPath) == loadedFilesWithoutXML.end()){
			textureID = loadTexture(sheetPath.c_str());
			loadedFilesWithoutXML[sheetPath] = textureID;
		}
		else{
			textureID = loadedFilesWithoutXML.at(sheetPath);
		}
		unsigned sheetWidth = loadedTextureSizes.at(textureID).first;
		unsigned sheetHeight = loadedTextureSizes.at(textureID).second;
		Texture* tex = new Texture(textureID, spriteCountX, spriteCountY, sheetWidth, sheetHeight, index, layer);
		return tex;
	}
	else if (textureType == UNEVEN_SPRITESHEET){
		detailsNode = textureNode->first_node("UnevenSpritesheet");
		string sheetName = detailsNode->first_attribute("sheetName")->value();
		string atlasName = detailsNode->first_attribute("atlasName")->value();
		directory = directory + detailsNode->first_attribute("Directory")->value();
		string key = directory + atlasName + sheetName;
		int layer = stoi(textureNode->first_attribute("layer")->value());
		xml_document<>* xmlDoc;
		if (loadedFiles.find(key) == loadedFiles.end()){
			string sheetPath = directory + sheetName;
			string atlasPath = directory + atlasName;
			textureID = loadTexture(sheetPath.c_str());
			xmlDoc = loadXMLFile(atlasPath.c_str());
			loadedFiles[key] = make_pair(textureID, xmlDoc);
		}
		else{
			textureID = loadedFiles[key].first;
			xmlDoc = loadedFiles[key].second;
		}
		string spriteName = detailsNode->first_attribute("spriteName")->value();
		float size = stof(detailsNode->first_attribute("size")->value());

		Texture* tex = new Texture(textureID, xmlDoc, spriteName, size, layer, loadedTextureSizes.at(textureID).first, loadedTextureSizes.at(textureID).second);
		return tex;
	}
	else{
		throw "Uknown texture type in XML file!";
	}
}

inline Animation* enrichAnimationInformation(xml_node<>* animationNode){
	bool loop = false;
	bool collides = true;
	ANIMATION_TYPE animationType = ANIMATION_IDLE;
	unsigned startingIndex = 0, endingIndex = 0;
	unsigned framesPerImage = 1;
	if (animationNode->first_attribute("loop") != nullptr){
		loop = stoi(animationNode->first_attribute("loop")->value()) == 1;
	}

	if (animationNode->first_attribute("type") != nullptr){
		animationType = static_cast<ANIMATION_TYPE>(stoi(animationNode->first_attribute("type")->value()));
	}

	if (animationNode->first_attribute("startingIndex") != nullptr){
		startingIndex = stoi(animationNode->first_attribute("startingIndex")->value());
	}

	if (animationNode->first_attribute("endingIndex") != nullptr){
		endingIndex = stoi(animationNode->first_attribute("endingIndex")->value());
	}

	if (animationNode->first_attribute("animationCollides") != nullptr){
		collides = stoi(animationNode->first_attribute("animationCollides")->value()) == 1;
	}

	if (animationNode->first_attribute("framesPerImage") != nullptr){
		framesPerImage = stoi(animationNode->first_attribute("framesPerImage")->value());
	}

	Animation* anim = new Animation(animationType, startingIndex, endingIndex);
	anim->setDoLoop(loop);
	anim->setAnimationCollides(collides);
	anim->setFramesPerImage(framesPerImage);
	Texture* tex;
	if (animationNode->first_node("Texture") != nullptr){
		tex = enrichTextureInformation(animationNode->first_node("Texture"));
		anim->setTexture(tex);
	}
	return anim;
}

inline ParticleEmitter* enrichParticleEmitterInformation(xml_node<>* emitterNode, float posX = 0, float posY = 0, float posZ = 0){
	EMITTER_TYPE emitterType = CONSTANT_EMITTER;
	float maxLifeTime = 0;
	float startSize = 1, endSize = 1;
	int maximumParticleCount = 0;
	Texture* particleTexture = nullptr;
	Vector3 position, velocity, velocityDeviation;
	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;
	Color endColor, startColor;
	Vector3 offsetPosition;
	offsetPosition.x = posX;
	offsetPosition.y = posY;
	offsetPosition.z = posZ;
	startColor.a = 1.0;
	startColor.b = 1.0;
	startColor.r = 1.0;
	startColor.g = 1.0;

	endColor = startColor;

	xml_node<>* currentDetail = emitterNode->first_node("emitterType");
	if (currentDetail == nullptr){
		throw "No emitter type!";
	}
	emitterType = static_cast<EMITTER_TYPE>(stoi(currentDetail->value()));

	currentDetail = emitterNode->first_node("maxLifeTime");
	if (currentDetail == nullptr){
		throw "No max lifetime!";
	}
	maxLifeTime = stof(currentDetail->value());

	currentDetail = emitterNode->first_node("startSize");
	if (currentDetail != nullptr){
		startSize = stof(currentDetail->value());
	}

	currentDetail = emitterNode->first_node("endSize");
	if (currentDetail != nullptr){
		endSize = stof(currentDetail->value());
	}

	currentDetail = emitterNode->first_node("maximumParticleCount");
	if (currentDetail == nullptr){
		throw "No maximum particle count!";
	}
	maximumParticleCount = stoi(currentDetail->value());

	currentDetail = emitterNode->first_node("Position");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			position.x = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			position.y = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			position.z = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = emitterNode->first_node("velocityDeviation");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			velocityDeviation.x = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			velocityDeviation.y = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			velocityDeviation.z = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = emitterNode->first_node("startColor");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("r") != nullptr){
			startColor.r = stof(currentDetail->first_attribute("r")->value());
		}
		if (currentDetail->first_attribute("g") != nullptr){
			startColor.g = stof(currentDetail->first_attribute("g")->value());
		}
		if (currentDetail->first_attribute("b") != nullptr){
			startColor.b = stof(currentDetail->first_attribute("b")->value());
		}
		if (currentDetail->first_attribute("a") != nullptr){
			startColor.a = stof(currentDetail->first_attribute("a")->value());
		}
	}
	
	currentDetail = emitterNode->first_node("endColor");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("r") != nullptr){
			endColor.r = stof(currentDetail->first_attribute("r")->value());
		}
		if (currentDetail->first_attribute("g") != nullptr){
			endColor.g = stof(currentDetail->first_attribute("g")->value());
		}
		if (currentDetail->first_attribute("b") != nullptr){
			endColor.b = stof(currentDetail->first_attribute("b")->value());
		}
		if (currentDetail->first_attribute("a") != nullptr){
			endColor.a = stof(currentDetail->first_attribute("a")->value());
		}
	}

	currentDetail = emitterNode->first_node("Texture");
	if (currentDetail != nullptr){
		particleTexture = enrichTextureInformation(currentDetail);
	}

	ParticleEmitter* tmp = new ParticleEmitter();
	tmp->emitterType = emitterType;
	tmp->maxLifeTime = maxLifeTime;
	tmp->startSize = startSize;
	tmp->endSize = endSize;
	tmp->maximumParticleCount = maximumParticleCount;
	tmp->particleTexture = particleTexture;
	tmp->textured = particleTexture != nullptr;
	tmp->position = position;
	tmp->velocity = velocity;
	tmp->velocityDeviation = velocityDeviation;
	tmp->startColor = startColor;
	tmp->endColor = endColor;
	tmp->offsetPosition = offsetPosition;
	return tmp;
}

inline Mix_Chunk* loadSound(const char* fileLocation){
	static std::unordered_map<std::string, Mix_Chunk*> loadedSounds;
	Mix_Chunk* sound = new Mix_Chunk();
	if (loadedSounds.size() == 0 || loadedSounds.find(fileLocation) == loadedSounds.end()){
		sound = Mix_LoadWAV(fileLocation);
		loadedSounds[fileLocation] = sound;
		auto x = Mix_GetError();
		int y = 5;
	}
	else{
		sound = loadedSounds[fileLocation];
	}
	return sound;
}

inline Mix_Music* loadMusic(const char* fileLocation){
	static std::unordered_map<std::string, Mix_Music*> loadedMusic;
	Mix_Music* music;
	if (loadedMusic.size() == 0 || loadedMusic.find(fileLocation) == loadedMusic.end()){
		music = Mix_LoadMUS(fileLocation);
		loadedMusic[fileLocation] = music;
	}
	else{
		music = loadedMusic[fileLocation];
	}
	return music;
}

inline GameSound* enrichSoundInformation(xml_node<>* soundNode){
	string fileLocation = RESOURCE_FOLDER;
	SOUND_TYPE soundType;
	DIRECTION collisionDirection;
	COLLISION_STRENGTH collisionStrength;
	SOUND_TRIGGER soundTrigger;
	int loopCount = 0;
	Mix_Chunk* sound;
	GameSound* gameSound = new GameSound();

	if (soundNode->first_attribute("directory") == nullptr){
		throw "Directory attribute not found!";
	}
	fileLocation = fileLocation + soundNode->first_attribute("directory")->value();
	if (soundNode->first_attribute("fileName") == nullptr){
		throw "Filename attribute not found!";
	}
	fileLocation = fileLocation + soundNode->first_attribute("fileName")->value();
	sound = loadSound(fileLocation.c_str());
	gameSound->setSound(sound);

	if (soundNode->first_attribute("soundType") == nullptr){
		throw "No sound type!";
	}
	soundType = static_cast<SOUND_TYPE>(stoi(soundNode->first_attribute("soundType")->value()));
	gameSound->setSoundType(soundType);

	if (soundType == TRIGGER_SOUND){
		if (soundNode->first_attribute("soundTrigger") == nullptr){
			throw "No trigger for triggered sound!";
		}
		soundTrigger = static_cast<SOUND_TRIGGER>(stoi(soundNode->first_attribute("soundTrigger")->value()));
		gameSound->setSoundTrigger(soundTrigger);
	}

	if (soundType == COLLISION_SOUND){
		if (soundNode->first_attribute("direction") == nullptr){
			throw "No direction for collision sound!";
		}
		collisionDirection = static_cast<DIRECTION>(stoi(soundNode->first_attribute("direction")->value()));
		gameSound->setCollisionDirection(collisionDirection);

		if (soundNode->first_attribute("collisionStrength") == nullptr){
			throw "No strength found for collision sound!";
		}
		collisionStrength = static_cast<COLLISION_STRENGTH>(stoi(soundNode->first_attribute("collisionStrength")->value()));
		gameSound->setCollisionStrength(collisionStrength);
	}

	if (soundNode->first_attribute("loopCount") != nullptr){
		loopCount = stoi(soundNode->first_attribute("loopCount")->value());
	}
	gameSound->setLoops(loopCount);
	return gameSound;
}

inline Entity* enrichEntityInformation(xml_node<>* entityNode){
	if (entityNode == nullptr){
		return nullptr;
	}
	Texture* tex = nullptr;
	if (entityNode->first_node("Texture") != nullptr){
		tex = enrichTextureInformation(entityNode->first_node("Texture"));
	}
	string entityID = entityNode->first_node("EntityID")->value();
	Entity* entity = new Entity(entityID, tex);
	entity->setIsAnimated(false);
	if (entityNode->first_node("Animations") != nullptr){
		entity->setIsAnimated(true);
		xml_node<>* animationNode = entityNode->first_node("Animations")->first_node("Animation");
		do{
			Animation* animation = enrichAnimationInformation(animationNode);
			entity->addAnimation(animation);
			if (animationNode->first_attribute("Starting") != nullptr && stoi(animationNode->first_attribute("Starting")->value()) == 1){
				entity->startAnimation(animation->getAnimationType());
			}
			animationNode = animationNode->next_sibling();
		} while (animationNode != nullptr);
	}
	xml_node<>* detailsNode = entityNode->first_node("Details");
	if (detailsNode == nullptr){
		throw "Empty subEntity tag!";
	}

	float posX = 0, posY = 0, posZ = 0;
	float velX = 0, velY = 0, velZ = 0;
	float accX = 0, accY = 0, accZ = 0;
	float scaleX = 1, scaleY = 1, scaleZ = 1;
	float rotation = 0;
	float rotationalVelocity = 0;
	bool canCollide = false;
	bool doRender = true;
	bool doMirror = false;
	BOUNDING_TYPE boundingType = SQUARE;

	xml_node<>* currentDetail = detailsNode->first_node("Position");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			posX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			posY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			posZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Velocity");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			velX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			velY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			velZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Acceleration");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			accX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			accY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			accZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Scale");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			scaleX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			scaleY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			scaleZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Rotation");
	if (currentDetail != nullptr){
		rotation = (M_PI / 180) * stof(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("rotationalVelocity");
	if (currentDetail != nullptr){
		rotationalVelocity = (M_PI / 180) * stof(currentDetail->value());
	}
	else{
		rotationalVelocity = 0;
	}

	currentDetail = detailsNode->first_node("canCollide");
	if (currentDetail != nullptr){
		canCollide = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("doRender");
	if (currentDetail != nullptr){
		doRender = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("boundingType");
	if (currentDetail != nullptr){
		boundingType = static_cast<BOUNDING_TYPE>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("doMirror");
	if (currentDetail != nullptr){
		doMirror = stoi(currentDetail->value()) == 1;
	}

	entity->setPosition(posX, posY, posZ);
	entity->setVelocity(velX, velY, velZ);
	entity->setAcceleration(accX, accY, accZ);
	entity->setScale(scaleX, scaleY, scaleZ);
	entity->setRotation(rotation);
	entity->setRotationalVelocity(rotationalVelocity);
	entity->setCanCollide(canCollide);
	entity->setDoRender(doRender);
	entity->setBoundingType(boundingType);
	entity->setDoMirror(doMirror);

	if (entityNode->first_node("Sibling") != nullptr){
		entity->setSibling(enrichEntityInformation(entityNode->first_node("Sibling")));
	}

	if (entityNode->first_node("Child") != nullptr){
		entity->setChild(enrichEntityInformation(entityNode->first_node("Child")));
	}

	return entity;
}

inline CompositeEntity* enrichXMLData(xml_node<>* rootNode){
	ENTITY_TYPE entityType = static_cast<ENTITY_TYPE>(stoi(rootNode->first_node("Type")->value()));
	CompositeEntity* tmp;
	tmp = new CompositeEntity();

	float posX = 0, posY = 0, posZ = 0;
	float velX = 0, velY = 0, velZ = 0;
	float accX = 0, accY = 0, accZ = 0;
	float scaleX = 1, scaleY = 1, scaleZ = 1;
	float rotation = 0;
	float rotationalVelocity = 0;
	float firingDelay = 0;
	bool canCollide = false, canCollideWithTiles = false, isInvincible = false, isActive = true, falls = false, isStatic = false;
	float topSpeed = 1.0;
	float textSize = 1.0, textSpacing = 1.0;
	unsigned layer = 0;
	float jumpSpeed = 0;
	int awarenessRadius = 160;
	int hidingRadius = 48;
	string displayText = "";
	string textSheetDirectory;
	GLuint textSheet = 1;
	BOUNDING_TYPE boundingType = SQUARE;
	COLLISION_BEHAVIOR collisionBehavior = NOTHING;
	TILE_COLLISION_BEHAVIOR tileCollisionBehavior = T_STOP;
	TILE_LOGIC_BEHAVIOR tileLogicBehavior = NO_LOGIC;
	BOUNDARY_BEHAVIOR boundaryBehavior = BOUND_STOP;
	tmp->setEntityType(entityType);
	xml_node<>* detailsNode = rootNode->first_node("Details");
	if (detailsNode == nullptr){
		throw "Error: Empty Entity tag!!";
	}
	xml_node<>* currentDetail = detailsNode->first_node("firingDelay");
	if (currentDetail != nullptr){
		firingDelay = stof(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("tileCollisionBehavior");
	if (currentDetail != nullptr){
		tileCollisionBehavior = static_cast<TILE_COLLISION_BEHAVIOR>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("Position");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			posX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			posY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			posZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Velocity");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			velX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			velY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			velZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Acceleration");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			accX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			accY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			accZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("Scale");
	if (currentDetail != nullptr){
		if (currentDetail->first_attribute("x") != nullptr){
			scaleX = stof(currentDetail->first_attribute("x")->value());
		}
		if (currentDetail->first_attribute("y") != nullptr){
			scaleY = stof(currentDetail->first_attribute("y")->value());
		}
		if (currentDetail->first_attribute("z") != nullptr){
			scaleZ = stof(currentDetail->first_attribute("z")->value());
		}
	}

	currentDetail = detailsNode->first_node("topSpeed");
	if (currentDetail != nullptr){
		topSpeed = stof(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("jumpSpeed");
	if (currentDetail != nullptr){
		jumpSpeed = stof(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("Rotation");
	if (currentDetail != nullptr){
		rotation = (M_PI / 180) * stof(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("rotationalVelocity");
	if (currentDetail != nullptr){
		rotationalVelocity = (M_PI / 180) * stof(currentDetail->value());
	}
	else{
		rotationalVelocity = 0;
	}

	currentDetail = detailsNode->first_node("canCollide");
	if (currentDetail != nullptr){
		canCollide = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("isActive");
	if (currentDetail != nullptr){
		isActive = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("falls");
	if (currentDetail != nullptr){
		falls = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("isInvincible");
	if (currentDetail != nullptr){
		isInvincible = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("CollisionBehavior");
	if (currentDetail != nullptr){
		collisionBehavior = static_cast<COLLISION_BEHAVIOR>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("BoundaryBehavior");
	if (currentDetail != nullptr){
		boundaryBehavior = static_cast<BOUNDARY_BEHAVIOR>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("BoundingType");
	if (currentDetail != nullptr){
		boundingType = static_cast<BOUNDING_TYPE>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("TileLogicBehavior");
	if (currentDetail != nullptr){
		tileLogicBehavior = static_cast<TILE_LOGIC_BEHAVIOR>(stoi(currentDetail->value()));
	}

	currentDetail = detailsNode->first_node("DisplayText");
	if (currentDetail != nullptr){
		displayText = currentDetail->value();

		textSize = stof(currentDetail->first_attribute("size")->value());
		textSpacing = stof(currentDetail->first_attribute("spacing")->value());
	}

	currentDetail = detailsNode->first_node("TextSheet");
	if (currentDetail != nullptr){
		textSheetDirectory = RESOURCE_FOLDER;
		textSheetDirectory = textSheetDirectory + currentDetail->first_attribute("sheetPath")->value() + currentDetail->first_attribute("sheetName")->value();
		textSheet = loadTexture(textSheetDirectory.c_str());
	}

	currentDetail = detailsNode->first_node("isStatic");
	if (currentDetail != nullptr){
		isStatic = stoi(currentDetail->value()) == 1;
	}

	currentDetail = detailsNode->first_node("Layer");
	if (currentDetail != nullptr){
		layer = stoi(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("ParticleEmitters");
	if (currentDetail != nullptr){
		xml_node<>* particleEmitterNode = currentDetail->first_node("Emitter");
		while (particleEmitterNode != nullptr){
			tmp->addParticleEmitter(enrichParticleEmitterInformation(particleEmitterNode, posX, posY, posZ));
			particleEmitterNode = particleEmitterNode->next_sibling("Emitter");
		}
	}

	currentDetail = detailsNode->first_node("CollisionSounds");
	if (currentDetail != nullptr){
		xml_node<>* soundNode = currentDetail->first_node("Sound");
		while (soundNode != nullptr){
			tmp->addCollisionSound(enrichSoundInformation(soundNode));
			soundNode = soundNode->next_sibling("Sound");
		}
	}

	currentDetail = detailsNode->first_node("TriggerSounds");
	if (currentDetail != nullptr){
		xml_node<>* soundNode = currentDetail->first_node("Sound");
		while (soundNode != nullptr){
			tmp->addTriggerSound(enrichSoundInformation(soundNode));
			soundNode = soundNode->next_sibling("Sound");
		}
	}

	currentDetail = detailsNode->first_node("awarenessRadius");
	if (currentDetail != nullptr){
		awarenessRadius = stoi(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("hidingRadius");
	if (currentDetail != nullptr){
		hidingRadius = stoi(currentDetail->value());
	}

	currentDetail = detailsNode->first_node("canCollideWithTile");
	if (currentDetail != nullptr){
		canCollideWithTiles = stoi(currentDetail->value()) == 1;
	}
	else{
		canCollideWithTiles = canCollide;
	}

	tmp->setStartingPosition(posX, posY, posZ);
	tmp->setStartingVelocity(velX, velY, velZ);
	tmp->setTopSpeed(topSpeed);
	tmp->setStartingScale(scaleX, scaleY, scaleZ);
	tmp->setStartingAcceleration(accX, accY, accZ);
	tmp->setStartingRotation(rotation);
	tmp->setStartingRotationalVelocity(rotationalVelocity);
	tmp->setFiringDelay(firingDelay);
	tmp->setCanCollide(canCollide);
	tmp->setIsActive(isActive);
	tmp->setFalls(falls);
	tmp->setisInvincible(isInvincible);
	tmp->setCollisionBehavior(collisionBehavior);
	tmp->setBoundaryBehavior(boundaryBehavior);
	tmp->setBoundingType(boundingType);
	tmp->setDisplayText(displayText);
	tmp->setSize(textSize);
	tmp->setSpacing(textSpacing);
	tmp->setTextSheet(textSheet);
	tmp->setTileCollisionBehavior(tileCollisionBehavior);
	tmp->setTileLogicBehavior(tileLogicBehavior);
	tmp->setJumpSpeed(jumpSpeed);
	tmp->setIsStatic(isStatic);
	tmp->setAwarenessRadius(awarenessRadius);
	tmp->setHidingRadius(hidingRadius);
	tmp->setCanCollideWithTiles(canCollideWithTiles);
	if (rootNode->first_node("subEntities") != nullptr){
		tmp->setEntities(enrichEntityInformation(rootNode->first_node("subEntities")->first_node("First")));
	}
	tmp->setRenderLayer(layer);
	tmp->setObjectLayer(layer);
	tmp->updateBounding();

	xml_node<>* projectileTextureNode = rootNode->first_node("Projectile");
	if (projectileTextureNode != nullptr){
		CompositeEntity* compProjectile = new CompositeEntity();
		Entity* projectile = new Entity();
		projectile->setEntityID("Projectile");
		if (entityType == ACTOR_PLAYER){
			compProjectile->setEntityType(PLAYER_PROJECTILE);
		}
		else{
			compProjectile->setEntityType(ENEMY_PROJECTILE);
		}
		projectile->setPosition(0, 0, 0);
		projectile->setVelocity(0, 0, 0);
		projectile->setAcceleration(0, 0, 0);
		projectile->setScale(1, 1, 1);
		projectile->setBoundingType(SQUARE);
		projectile->setRotation(0);
		projectile->setCanCollide(true);
		projectile->setDoRender(true);

		compProjectile->setCanCollide(true);
		compProjectile->setFalls(false);
		compProjectile->setBoundingType(SQUARE);
		compProjectile->setCollisionBehavior(DESTROY);
		compProjectile->setBoundaryBehavior(BOUND_DESTROY_NO_ANIM);
		compProjectile->setScale(16, 16);

		if (projectileTextureNode->first_node("Texture") != nullptr){
			projectile->setIsAnimated(false);
			Texture* projectileTexture = enrichTextureInformation(projectileTextureNode->first_node("Texture"));
			projectile->setTexture(projectileTexture);
		}
		else if (projectileTextureNode->first_node("Animations") != nullptr && projectileTextureNode->first_node("Animations")->first_node("Animation") != nullptr){
			projectile->setIsAnimated(true);
			xml_node<>* animationNode = projectileTextureNode->first_node("Animations")->first_node("Animation");
			do{
				Animation* animation = enrichAnimationInformation(animationNode);
				projectile->addAnimation(animation);
				if (animationNode->first_attribute("Starting") != nullptr){
					projectile->startAnimation(animation->getAnimationType());
				}
				animationNode = animationNode->next_sibling();
			} while (animationNode != nullptr);
		}
		else{
			delete projectile;
			projectile = nullptr;
			throw "Error: No animation or texture for projectile!";
		}

		if (projectileTextureNode->first_node("TriggerSounds") != nullptr){
			xml_node<>* soundNode = projectileTextureNode->first_node("TriggerSounds")->first_node("Sound");
			while (soundNode != nullptr){
				compProjectile->addTriggerSound(enrichSoundInformation(soundNode));
				soundNode = soundNode->next_sibling("Sound");
			}
		}

		if (projectileTextureNode->first_node("CollisionSounds") != nullptr){
			xml_node<>* soundNode = projectileTextureNode->first_node("CollisionSounds")->first_node("Sound");
			while (soundNode != nullptr){
				compProjectile->addCollisionSound(enrichSoundInformation(soundNode));
				soundNode = soundNode->next_sibling("Sound");
			}
		}
		compProjectile->setEntities(projectile);
		compProjectile->setProjectile(nullptr);
		tmp->setProjectile(compProjectile);
	}
	else{
		tmp->setProjectile(nullptr);
	}
	return tmp;
}

inline CollisionListener* enrichCollisionInformation(xml_node<>* collisionNode){
	unsigned entityType1, entityType2;

	COLLISION_BEHAVIOR behaviorOfEntity1 = COLLISION_BEHAVIOR_SIZE, behaviorOfEntity2 = COLLISION_BEHAVIOR_SIZE;

	GameSound* soundToPlay = nullptr;
	bool up = true, down = true, left = true, right = true;

	if (collisionNode->first_attribute("entityType1") == nullptr){
		throw "Entity type not passed!";
	}
	entityType1 = stoi(collisionNode->first_attribute("entityType1")->value());

	if (collisionNode->first_attribute("entityType2") == nullptr){
		throw "Entity type not passed!";
	}
	entityType2 = stoi(collisionNode->first_attribute("entityType2")->value());

	if (collisionNode->first_attribute("collisionBehavior1") != nullptr){
		behaviorOfEntity1 = static_cast<COLLISION_BEHAVIOR>(stoi(collisionNode->first_attribute("collisionBehavior1")->value()));
	}

	if (collisionNode->first_attribute("collisionBehavior2") != nullptr){
		behaviorOfEntity2 = static_cast<COLLISION_BEHAVIOR>(stoi(collisionNode->first_attribute("collisionBehavior2")->value()));
	}

	if (collisionNode->first_attribute("up") != nullptr){
		up = stoi(collisionNode->first_attribute("up")->value()) == 1;
	}

	if (collisionNode->first_attribute("down") != nullptr){
		down = stoi(collisionNode->first_attribute("down")->value()) == 1;
	}

	if (collisionNode->first_attribute("left") != nullptr){
		left = stoi(collisionNode->first_attribute("left")->value()) == 1;
	}

	if (collisionNode->first_attribute("right") != nullptr){
		right = stoi(collisionNode->first_attribute("right")->value()) == 1;
	}

	if (collisionNode->first_node("Sound") != nullptr){
		soundToPlay = enrichSoundInformation(collisionNode->first_node("Sound"));
	}

	CollisionListener* toAdd = new CollisionListener(entityType1, entityType2, behaviorOfEntity1, behaviorOfEntity2, down, up, left, right, soundToPlay);
	return toAdd;
}

inline void loadXMLData(GameEngine& engine){
	DIR *dirp;
	struct dirent *dp;
	char fileName[360];
	char fileDirec[360] = RESOURCE_FOLDER"Assets/XML/";
	if ((dirp = opendir("./Assets/XML")) == NULL){
		throw "Could not find current directory!";
	}
	while ((dp = readdir(dirp)) != NULL){
		strcpy_s(fileName, dp->d_name);
		if (fileName[0] != '.' && fileName[1] != '.' && strcmp(fileName, "Objects") != 0){
			strcpy_s(fileDirec, RESOURCE_FOLDER"Assets/XML/");
			strcat_s(fileDirec, fileName);
			xml_document<>* doc = loadXMLFile(fileDirec);
			if (doc->first_node("Entities") != nullptr){
				xml_node<>* entityNode = doc->first_node()->first_node();
				do{
					CompositeEntity* toAdd = enrichXMLData(entityNode);
					if (entityTypes.find(toAdd->getEntityType()) != entityTypes.end()){
						throw "Entity type already exists!";
					}
					entityTypes[toAdd->getEntityType()] = toAdd;
					entityNode = entityNode->next_sibling();
				} while (entityNode != nullptr);
			}
			if (doc->first_node("Collisions") != nullptr){
				xml_node<>* collisionNode = doc->first_node()->first_node();
				do{
					CollisionListener* toAdd = enrichCollisionInformation(collisionNode);
					engine.addCollisionEvent(toAdd);
					collisionNode = collisionNode->next_sibling();
				} while (collisionNode != nullptr);
			}
		}
	}
}

inline void readEntityData(std::ifstream &stream, Level* level, GameEngine& engine) {
	string line;
	ENTITY_TYPE type;
	float placeX, placeY;
	float rotation = 0;
	float rotationalVelocity = 0;
	unsigned renderLayer, objectLayer;
	CompositeEntity* entity = new CompositeEntity();
	while (getline(stream, line)) {
		if (line == "") {
			break;
		}
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = static_cast<ENTITY_TYPE>(stoi(value));
			if (entityTypes.find(type) == entityTypes.end() && type != STATIC_ENTITY && type != STATIC_FALL_ON_COLLDE){
				throw "Entity type undefined!";
			}
			if (type != STATIC_ENTITY && type != STATIC_FALL_ON_COLLDE){
				entity->deepCopy(entityTypes[type]);
			}
		}
		else if (key == "location") {
			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			placeX = (atoi(xPosition.c_str()) + 0.5)*level->getTileSize();
			placeY = (atoi(yPosition.c_str()) - 0.5)*-(level->getTileSize());
			entity->setStartingPosition(placeX, placeY);
		}
		else if (key == "text"){
			string text = value;
			entity->setDisplayText(text);
		}
		else if (key == "size"){
			float size = stof(value);
			entity->setSize(size);
		}
		else if (key == "spacing"){
			float spacing = stof(value);
			entity->setSpacing(spacing);
		}
		else if (key == "renderLayer"){
			renderLayer = stoi(value);
			entity->setRenderLayer(renderLayer);
		}
		else if (key == "objectLayer"){
			objectLayer = stoi(value);
			entity->setObjectLayer(objectLayer);
		}
		else if (key == "warpDestination"){
			string destination = value;
			entity->setWarpDestination(destination);
		}
		else if (key == "xmlDescriber"){
			char xmlFile[360];
			strcpy_s(xmlFile, RESOURCE_FOLDER);
			strcat_s(xmlFile, "Assets/");
			strcat_s(xmlFile, value.c_str());
			xml_document<>* doc = loadXMLFile(xmlFile);
			delete entity;
			entity = nullptr;
			entity = enrichXMLData(doc->first_node("Entity"));
			entity->setRenderLayer(renderLayer);
			entity->setObjectLayer(objectLayer);
			entity->setStartingPosition(placeX, placeY);
			entity->setStartingRotation(rotation);
			entity->setStartingRotationalVelocity(rotationalVelocity);
		}
		else if (key == "rotation"){
			rotation = stof(value) * (M_PI / 180);
			entity->setStartingRotation(rotation);
		}
		else if (key == "rotationalVelocity"){
			rotationalVelocity = stof(value) * (M_PI / 180);
			entity->setStartingRotationalVelocity(rotationalVelocity);
		}
	}
	if (type == ACTOR_PLAYER){
		level->setPlayerEntity(entity);
	}
	engine.addGameEntity(level->getLevelID(), entity);
}

inline Level* loadLevel(const char* levelFile, const char* tileData, const char* levelName, const char* spriteSheetLocation, GameEngine& engine){
	Level* level = new Level();
	ifstream infile(levelFile);
	string line;
	level->setLevelID(levelName);
	//level->setTileSize(16);
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!level->readHeader(levelFile, infile)) {
				return nullptr;
			}
		}
		else if (line == "[layer]") {
			level->readLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			readEntityData(infile, level, engine);
		}
	}
	xml_document<>* doc = loadXMLFile(tileData);
	xml_node<>* node = doc->first_node();
	GLuint sheet = loadTexture(spriteSheetLocation);
	level->setTileSet(sheet);
	level->fillSpriteSheetData(node);
	return level;
}

inline void loadLevelData(GameEngine& engine){
	char spritesFolder[] = RESOURCE_FOLDER"Assets/Levels/Spritesheets/";
	char levelsFolder[] = RESOURCE_FOLDER"Assets/Levels/";
	char txtFile[360];
	char xmlFile[360];
	char spriteSheetName[360];
	char levelName[360];
	char spriteSheetDirec[360];
	//float r = 0, g = 0, b = 0, a = 1.0;
	xml_document<>* levelIndex = loadXMLFile(RESOURCE_FOLDER"Assets/Levels/level_index.xml");
	if (!levelIndex){
		throw "Level index not found!";
	}
	xml_node<>* levelNode = levelIndex->first_node();
	if (levelNode == nullptr){
		throw "Level index file empty!";
	}
	levelNode = levelNode->first_node("Level");
	if (levelNode == nullptr){
		throw "No level data!";
	}
	do{
		strcpy_s(spriteSheetDirec, spritesFolder);
		if (levelNode->first_attribute("id") == nullptr){
			throw "ID field missing for level!";
		}
		strcpy_s(levelName, levelNode->first_attribute("id")->value());
		strcpy_s(txtFile, levelsFolder);
		strcat_s(txtFile, levelName);
		strcat_s(txtFile, separator);
		strcat_s(txtFile, levelName);
		strcat_s(txtFile, txtExt);
		if (levelNode->first_attribute("spriteSheet") == nullptr){
			throw "Tile sheet name missing for level!";
		}
		strcpy_s(spriteSheetName, levelNode->first_attribute("spriteSheet")->value());
		strcpy_s(spriteSheetDirec, spritesFolder);
		strcat_s(spriteSheetDirec, spriteSheetName);

		strcpy_s(xmlFile, spritesFolder);
		strcat_s(xmlFile, spriteSheetName);
		strcat_s(xmlFile, tsxExt);

		strcat_s(spriteSheetDirec, pngExt);
		Level* level = loadLevel(txtFile, xmlFile, levelName, spriteSheetDirec, engine);
		//if (levelNode->first_node("Color") != nullptr){
		//	r = stof(levelNode->first_node("Color")->first_attribute("r")->value());
		//	g = stof(levelNode->first_node("Color")->first_attribute("g")->value());
		//	b = stof(levelNode->first_node("Color")->first_attribute("b")->value());
		//	a = stof(levelNode->first_node("Color")->first_attribute("a")->value());
		//}
		//level->setBackgroundColor(r, g, b, a);
		engine.addLevel(level);
		if (levelNode->first_attribute("startingLevel") != nullptr){
			engine.setLevel(levelName);
		}
		levelNode = levelNode->next_sibling();
	} while (levelNode != nullptr);
}

#endif