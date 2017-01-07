#include "Texture.h"
#include "ShaderProgram.h"
using namespace std;
using namespace rapidxml;


Texture::Texture(){
	this->textureID = 0;
	textureType = TEXTURE_TYPE_COUNT;
}

Texture::Texture(GLuint textureID, unsigned textureLayer){
	this->textureID = textureID;
	textureType = IMAGE;
	this->textureLayer = textureLayer;
	xmlDoc = nullptr;
}

Texture::Texture(GLuint textureID, unsigned spriteCountX, unsigned spriteCountY, unsigned sheetWidth, unsigned sheetHeight, unsigned startingIndex, unsigned textureLayer){
	this->textureID = textureID;
	textureType = EVEN_SPRITESHEET;
	this->spriteCountX = spriteCountX;
	this->spriteCountY = spriteCountY;
	this->textureLayer = textureLayer;
	this->sheetWidth = sheetWidth;
	this->sheetHeight = sheetHeight;
	spriteWidth = sheetWidth / spriteCountX;
	spriteHeight = sheetHeight / spriteCountY;
	index = startingIndex;
	xmlDoc = nullptr;
}

Texture::Texture(GLuint textureID, xml_document<>* xmlDoc, const string& spriteName, float size, unsigned textureLayer, float sheetWidth, float sheetHeight){
	if (xmlDoc == nullptr){
		throw "xmlDoc is nullptr!";
	}
	this->textureID = textureID;
	this->textureType = UNEVEN_SPRITESHEET;
	this->xmlDoc = xmlDoc;
	this->spriteName = spriteName;
	this->size = size;
	this->sheetWidth = sheetWidth;
	this->sheetHeight = sheetHeight;
	this->textureLayer = textureLayer;
	if (spriteName != ""){
		bool foundSprite = false;
		xml_node<>* rootNode = xmlDoc->first_node();
		for (xml_node<>* subNode = rootNode->first_node(); subNode; subNode = subNode->next_sibling()){
			if (subNode->first_attribute("name")->value() == spriteName){
				this->x = stof(subNode->first_attribute("x")->value());
				this->y = stof(subNode->first_attribute("y")->value());
				this->spriteWidth = stof(subNode->first_attribute("width")->value());
				this->spriteHeight = stof(subNode->first_attribute("height")->value());
				foundSprite = true;
				break;
			}
		}
		if (!foundSprite){
			throw "Sprite not found - improper name!";
		}
	}
}

void Texture::setIndex(unsigned index){
	if (textureType != EVEN_SPRITESHEET){
		throw "Index is reserved for an even spritesheet!";
	}
	this->index = index;
}

void Texture::incrementIndex(){
	if (textureType != EVEN_SPRITESHEET){
		throw "Index is reserved for an even spritesheet!";
	}
	index++;
}

void Texture::decrementIndex(){
	if (textureType != EVEN_SPRITESHEET){
		throw "Index is reserved for an even spritesheet!";
	}
	index--;
}

void Texture::setXmlDoc(rapidxml::xml_document<>* xmlDoc){
	if (textureType != UNEVEN_SPRITESHEET){
		throw "xml document reserved for uneven spritehseets only!";
	}
	if (xmlDoc == nullptr){
		throw "Invalid document passed in: null document!";
	}
	this->xmlDoc = xmlDoc;
}

void Texture::setSpriteName(const string& spriteName){
	if (textureType != UNEVEN_SPRITESHEET || xmlDoc == nullptr){
		throw "Sprite name is reserved for use with an xml file!";
	}
	if (spriteName != ""){
		bool foundSprite = false;
		xml_node<>* rootNode = xmlDoc->first_node();
		for (xml_node<>* subNode = rootNode->first_node(); subNode; subNode = subNode->next_sibling()){
			if (subNode->first_attribute("name")->value() == spriteName){
				this->x = stof(subNode->first_attribute("x")->value());
				this->y = stof(subNode->first_attribute("y")->value());
				this->spriteWidth = stof(subNode->first_attribute("width")->value());
				this->spriteHeight = stof(subNode->first_attribute("height")->value());
				foundSprite = true;
				break;
			}
		}
		if (!foundSprite){
			throw "Sprite not found - improper name!";
		}
	}
}

void Texture::drawEvenSpriteSheetImage(){
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX; 
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY; 
	spriteWidth = 1.0 / (float)spriteCountX; 
	spriteHeight = 1.0 / (float)spriteCountY;
	std::vector<GLfloat> texCoords = { u, v + spriteHeight, u + spriteWidth, v, u, v,
		u + spriteWidth, v, u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};
	textureCoordinates = texCoords;
}

void Texture::drawUnevenSpriteSheetImage(){
	float u = x / sheetWidth;
	float v = y / sheetHeight;
	textureCoordinates = { u, v + spriteHeight / sheetHeight, u + spriteWidth / sheetWidth, v, u, v, u + spriteWidth / sheetWidth, v, u, v + spriteHeight / sheetHeight, u + spriteWidth / sheetWidth, v + spriteHeight / sheetHeight };
}

void Texture::unevenSpriteSheetImageObjectVertices(){
	float aspect = (spriteWidth / sheetWidth) / (spriteHeight / sheetHeight);
	objectVertices = {
		-0.5f * size * aspect, -0.5f * size, 0.5f * size * aspect, 0.5f * size, -0.5f * size * aspect, 0.5f * size, 0.5f * size * aspect, 0.5f * size, -0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size };
}

void Texture::drawText(){
	float texture_size = 1.0 / 16.0f; 
	vector<GLfloat> texCoordData;
	float texture_x = (float)(((int)textVal) % 16) / 16.0f; 
	float texture_y = (float)(((int)textVal) / 16) / 16.0f; 
	texCoordData = { texture_x, texture_y,
		texture_x, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x + texture_size, texture_y + texture_size, texture_x + texture_size, texture_y, texture_x, texture_y + texture_size,
	};
	textureCoordinates = texCoordData;
}

void Texture::textObjectVertices(){
	vector<GLfloat> vertexData;
	vertexData = { (-0.5f * size), 0.5f * size, (-0.5f * size), -0.5f * size, (0.5f * size), 0.5f * size, (0.5f * size), -0.5f * size, (0.5f * size), 0.5f * size,
	(-0.5f * size), -0.5f * size };
	objectVertices = vertexData;
}

float Texture::getSpriteWidth(){
	return spriteWidth;
}

float Texture::getSpriteHeight(){
	return spriteHeight;
}

float Texture::getSheetHeight(){
	return sheetHeight;
}

float Texture::getSheetWidth(){
	return sheetWidth;
}

float Texture::getSpriteSize(){
	return size;
}


float Texture::getTileSize(){
	if (textureType != EVEN_SPRITESHEET){
		throw "Only for even spritesheets!";
	}
	return (sheetWidth / spriteCountX);
}

vector<GLfloat>& Texture::getTextureCoordinates(){
	vector<GLfloat> defaultVertices = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	switch(textureType){
	case IMAGE:
		textureCoordinates = defaultVertices;
		break;
	case EVEN_SPRITESHEET:
		drawEvenSpriteSheetImage();
		break;
	case UNEVEN_SPRITESHEET:
		drawUnevenSpriteSheetImage();
		break;
	default:
		throw "Unknown texture type!";
		break;
	}
	return textureCoordinates;
}

vector<GLfloat>& Texture::getObjectCoordinates(){
	if (textureType == UNEVEN_SPRITESHEET){
		unevenSpriteSheetImageObjectVertices();
	}
	else{
		throw "Only text type should return a set of object vertices!";
	}
	return objectVertices;
}

unsigned Texture::getTextureType(){
	return textureType;
}

unsigned Texture::getTextureLayer(){
	return textureLayer;
}

GLuint Texture::getTextureID(){
	return textureID;
}

void Texture::setTextureLayer(unsigned textureLayer){
	this->textureLayer = textureLayer;
}

Texture::~Texture()
{
}

void Texture::deepCopy(Texture* toCopy){
	this->textureID = toCopy->textureID;
	this->sheetHeight = toCopy->sheetHeight;
	this->sheetWidth = toCopy->sheetWidth;
	this->size = toCopy->size;
	this->spacing = toCopy->spacing;
	this->spriteCountX = toCopy->spriteCountX;
	this->spriteCountY = toCopy->spriteCountY;
	this->spriteHeight = toCopy->spriteHeight;
	this->spriteWidth = toCopy->spriteWidth;
	this->xmlDoc = toCopy->xmlDoc;
	this->textureLayer = toCopy->textureLayer;
	this->textureType = toCopy->textureType;
	this->spriteName = toCopy->spriteName;
	this->x = toCopy->x;
	this->y = toCopy->y;
}