#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef _WINDOWS
#include<GL/glew.h>
#endif
#include "rapidxml.hpp"
#include <string>
#include <vector>

class ShaderProgram;

enum TEXTURE_TYPE { IMAGE, EVEN_SPRITESHEET, UNEVEN_SPRITESHEET, TEXTURE_TYPE_COUNT };

class Texture
{
protected:
	//For all texture types:
	GLuint textureID;
	TEXTURE_TYPE textureType;
	unsigned textureLayer;
	std::vector<GLfloat> textureCoordinates;
	std::vector<GLfloat> objectVertices;
	//Done

	//For EVEN_SPRITESHEET:
	unsigned index;
	unsigned spriteCountX;
	unsigned spriteCountY;
	void drawEvenSpriteSheetImage();
	//Done

	//For UNEVEN_SPRITESHEET:
	float x, y;
	float sheetWidth, sheetHeight;
	float spriteWidth, spriteHeight;
	std::string spriteName;
	rapidxml::xml_document<>* xmlDoc;
	void drawUnevenSpriteSheetImage();
	void unevenSpriteSheetImageObjectVertices();
	void evenSpriteSheetObjectVertices();
	//Done


	//For TEXT:
	char textVal;
	float spacing;
	void drawText();
	void textObjectVertices();
	//Done

	//For TEXT and UNEVEN_SPRITESHEET:
	float size;
	//Done

public:
	Texture();
	//Constructor for image texture type:
	Texture(GLuint textureID, unsigned textureLayer = 0);
	//Done

	//Constructor for even spritesheet texture type:
	Texture(GLuint textureID, unsigned spriteCountX, unsigned spriteCountY, unsigned sheetWidth, unsigned sheetHeight, unsigned startingIndex, unsigned textureLayer = 0);
	//Done

	//Constructor for uneven spritesheet texture type (WARNING: can throw exception if nullptr passed in for xmlDoc):
	Texture(GLuint textureID, rapidxml::xml_document<>* xmlDoc, const std::string& spriteName, float size, unsigned textureLayer, float sheetWidth, float sheetHeight);
	//Done

	//Constructor for text texture type:
	Texture(GLuint textureID, char textVal, float size, float spacing, unsigned textureLayer = 0);
	//Done

	//Functions for even spritesheet texture type:
	//setIndex recommended: Can result in an exception!
	void setIndex(unsigned index);
	void incrementIndex();
	void decrementIndex();
	//Done

	//Functions for uneven spritesheet texture type:
	//set xmlDoc will likely never be needed.
	void setXmlDoc(rapidxml::xml_document<>* xmlDoc);

	//Warning: can throw exception if spriteName is wrong!
	void setSpriteName(const std::string& spriteName);
	//Done

	//Functions for uneven text texture type:
	//Will likely never need to change the value of the character:
	void setCharacter(char textVal);
	//Done

	~Texture();

	//Shared Functions:
	float getSpriteWidth();
	float getSpriteHeight();
	float getSheetWidth();
	float getSheetHeight();
	float getSpriteSize();
	std::vector<GLfloat>& getTextureCoordinates();
	std::vector<GLfloat>& getObjectCoordinates();
	unsigned getTextureType();
	unsigned getTextureLayer();
	GLuint getTextureID();

	float getTileSize();

	void setTextureLayer(unsigned textureLayer);
	//Done

	virtual void deepCopy(Texture* toCopy);
};

#endif