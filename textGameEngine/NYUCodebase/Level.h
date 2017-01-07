#ifndef LEVEL_H
#define LEVEL_H
#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include "rapidxml.hpp"
#include "CompositeEntity.h"
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

class Tile;
class Level
{
protected:
	std::unordered_map<unsigned, unsigned char**> levelData;
	std::unordered_map<unsigned, std::vector<std::vector<Tile*>>> levelDataTiles;
	//std::unordered_map<unsigned, GLuint> layerSpriteSheet;
	unsigned mapHeight, mapWidth;

	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);

	GLuint tileSet;
	unsigned spriteCountX, spriteCountY;
	float sheetWidth, sheetHeight;
	float tileWidth, tileHeight;
	float tileSize;
	std::string levelID;
	std::string spriteSheetName;
	std::unordered_map<unsigned, std::vector<float>> vertexData;
	std::unordered_map<unsigned, std::vector<float>> texCoordData;
	std::unordered_map<unsigned, std::vector<float>> colorData;

	static std::unordered_map<GLuint, std::unordered_map<unsigned, Tile*>> collisionData;
	
	CompositeEntity* playerEntity;

	void fillVertexArrays();
	void fillCollisionData(rapidxml::xml_node<>* tileNode);


	Color backgroundColor;

public:
	Level();
	Level(const char* stream, rapidxml::xml_document<>* doc, GLuint tileSet, unsigned spriteCountX, unsigned spriteCountY, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntities, std::unordered_map<unsigned, std::vector<CompositeEntity*>>& gameEntitiesRenderingOrder);
	~Level();

	void draw(ShaderProgram* program, unsigned layer);


	bool readHeader(const char* levelName, std::ifstream& stream);
	bool readLayerData(std::ifstream& stream);
	const std::string& getLevelID();
	unsigned getLayerCount();

	void setLevelID(const std::string& levelID);

	float getTileWidth();
	float getTileHeight();
	float getMapHeight();
	float getMapLength();
	float getTileSize();
	void setTileSet(GLuint tileSet);
	void setTileSize(float tileSize);
	void setPlayerEntity(CompositeEntity* playerEntity);
	void setBackgroundColor(float r, float g, float b, float a);
	Tile* getTile(unsigned layer, int gridX, int gridY);
	CompositeEntity* getPlayerEntity();
	Color getBackgroundColor();

	//const std::unordered_map<unsigned, unsigned char**> getLevelData();
	//const std::unordered_map<unsigned, GLuint> getLayerSpriteSheets();
	void fillSpriteSheetData(rapidxml::xml_node<>* tileNode);
	void fillTileTestData();

	void freeMemory();
};

#endif