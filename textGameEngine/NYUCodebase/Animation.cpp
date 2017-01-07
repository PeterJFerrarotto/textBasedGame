#include "Animation.h"
#include "Texture.h"

Animation::Animation(Animation* toCopy){
	deepCopy(toCopy);
}

Animation::Animation(ANIMATION_TYPE animationType, unsigned startingIndex, unsigned endingIndex) : animationType(animationType), startingIndex(startingIndex), endingIndex(endingIndex), currentIndex(startingIndex)
{
	framesPerImage = 1;
}

Animation::~Animation()
{
}

bool Animation::animationIsDone(){
	return !loop && currentIndex == endingIndex && currentImageFrameCount >= framesPerImage;
}

void Animation::updateTexture(){
	currentTexture->setIndex(currentIndex);
}

ANIMATION_TYPE Animation::getAnimationType(){
	return animationType;
}

Texture* Animation::getTexture(){
	return currentTexture;
}

bool Animation::getDoLoop(){
	return loop;
}

void Animation::setTexture(Texture* texture){
	currentTexture = texture;
}

void Animation::setDoLoop(bool loop){
	this->loop = loop;
}

void Animation::setAnimationCollides(bool collide){
	animationCollides = collide;
}

void Animation::runAnimation(float elapsed, float fps){
	animationElapsed += elapsed;
	if (animationElapsed > 1.0 / fps) {
		currentImageFrameCount++;
		if (currentImageFrameCount > framesPerImage){
			if (startingIndex <= endingIndex){
				currentIndex++;
				if (currentIndex > endingIndex){
					currentIndex = loop ? startingIndex : endingIndex;
					timesRun++;
				}
			}
			if (startingIndex > endingIndex){
				currentIndex--;
				if (currentIndex < endingIndex){
					currentIndex = loop ? startingIndex : endingIndex;
					timesRun++;
				}
			}
			updateTexture();
			animationElapsed = 0;
			currentImageFrameCount = 0;
		}
	}
}

void Animation::restartAnimation(){
	currentIndex = startingIndex;
	timesRun = 0;
	currentImageFrameCount = 0;
	updateTexture();
}

void Animation::deepCopy(Animation* toCopy){
	toCopy->restartAnimation();
	this->currentTexture = new Texture();
	this->currentTexture->deepCopy(toCopy->currentTexture);
	this->animationType = toCopy->animationType;
	this->currentIndex = toCopy->currentIndex;
	this->endingIndex = toCopy->endingIndex;
	this->loop = toCopy->loop;
	this->startingIndex = toCopy->startingIndex;
	this->timesRun = 0;
	this->animationCollides = toCopy->animationCollides;
	this->framesPerImage = toCopy->framesPerImage;
}

void Animation::freeMemory(){
	if (currentTexture != nullptr){
		try{
			currentTexture->getObjectCoordinates();
			delete currentTexture;
			currentTexture = nullptr;
		}
		catch (char* e){
			currentTexture = nullptr;
		}
	}
}

bool Animation::getAnimationCollides(){
	return animationCollides;
}

void Animation::setFramesPerImage(unsigned framesPerImage){
	this->framesPerImage = framesPerImage;
}