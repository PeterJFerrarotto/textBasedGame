#include "Level.h"
#include "ShaderProgram.h"
#include "Tile.h"
#include "mathHelper.h"
using namespace std;
using namespace rapidxml;

std::unordered_map<GLuint, std::unordered_map<unsigned, Tile*>> Level::collisionData = {};

Level::Level()
{
}

Level::~Level()
{
}

void Level::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / tileWidth); 
	*gridY = (int)(-worldY / tileHeight);
}

unsigned Level::getLayerCount(){
	return vertexData.size();
}

bool Level::readHeader(const char* levelName, std::ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") {
			break;
		}
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
		else if (key == "tilewidth"){
			tileWidth = atoi(value.c_str());
		}
		else if (key == "tileheight"){
			tileHeight = atoi(value.c_str());
		}
		else if (key == "backgroundColorR"){
			backgroundColor.r = (float)atoi(value.c_str()) / 255;
		}
		else if (key == "backgroundColorG"){
			backgroundColor.g = (float)atoi(value.c_str()) / 255;
		}
		else if (key == "backgroundColorB"){
			backgroundColor.b = (float)atoi(value.c_str()) / 255;
		}
		else if (key == "backgroundColorA"){
			backgroundColor.a = (float)atoi(value.c_str()) / 255;
		}
		else if (key == "backgroundColor"){
			int startHex = value.find('#') + 1;
			backgroundColor.a = hexToBase10(value.substr(startHex, 2)) / 255;
			backgroundColor.r = hexToBase10(value.substr(startHex + 2, 2)) / 255;
			backgroundColor.g = hexToBase10(value.substr(startHex + 4, 2)) / 255;
			backgroundColor.b = hexToBase10(value.substr(startHex + 6, 2)) / 255;
		}
	}
	if (tileWidth == tileHeight){
		tileSize = tileWidth;
	}
	return mapWidth != -1 && mapHeight != -1;
}

bool Level::readLayerData(std::ifstream &stream) {
	unsigned layer = levelData.size();
	levelData[layer] = new unsigned char*[mapHeight];
	for (int i = 0; i < mapHeight; ++i){
		levelData[layer][i] = new unsigned char[mapWidth];
	}
	string line;
	while (getline(stream, line)) {
		if (line == "") {
			break;
		}
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;
				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[layer][y][x] = val;
					}
					else {
						levelData[layer][y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

void Level::fillVertexArrays(){
	float colorRatio;
	for (size_t i = 0; i < levelData.size(); i++){
		if (i == levelData.size() - 1){
			colorRatio = 1;
		}
		else{
			colorRatio = 1 - (levelData.size() - i) * 0.125;
		}
		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {
				if ((int)(levelData[i][y][x]) != 0){
					int levelIndex = (int)levelData[i][y][x]-1;
					float u = (float)(((int)levelData[i][y][x] - 1) % spriteCountX) / (float)spriteCountX;
					float v = (float)(((int)levelData[i][y][x] - 1) / spriteCountX) / (float)spriteCountY;
					float spriteWidth = 1.0 / (float)spriteCountX;
					float spriteHeight = 1.0 / (float)spriteCountY;
					vertexData[i].insert(vertexData[i].end(), {
						tileSize * x, -tileSize * y, tileSize * x, (-tileSize * y) - tileSize, (tileSize * x) + tileSize, (-tileSize * y) - tileSize,
						tileSize * x, -tileSize * y, (tileSize * x) + tileSize, (-tileSize * y) - tileSize,
						(tileSize * x) + tileSize, -tileSize * y
					});
					texCoordData[i].insert(texCoordData[i].end(), { u, v,
						u, v + (spriteHeight),
						u + spriteWidth, v + (spriteHeight), u, v,
						u + spriteWidth, v + (spriteHeight), u + spriteWidth, v
					});
					for (int j = 0; j < 6; j++){
						colorData[i].insert(colorData[i].end(), { colorRatio, colorRatio, colorRatio, 1.0 });
					}
				}
			}
		}
	}
}

void Level::draw(ShaderProgram* program, unsigned layer){
	if (layer >= levelData.size())
		return;
	Matrix matrix;
	glBindTexture(GL_TEXTURE_2D, tileSet);
	program->setModelMatrix(matrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData[layer].data());

	glEnableVertexAttribArray(program->texCoordAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData[layer].data());

	GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, colorData[layer].data());
	glEnableVertexAttribArray(colorAttribute);

	glDrawArrays(GL_TRIANGLES, 0, vertexData[layer].size()/2);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(colorAttribute);
}


float Level::getTileWidth(){
	return tileWidth;
}

float Level::getTileHeight(){
	return tileHeight;
}

const std::string& Level::getLevelID(){
	return levelID;
}

void Level::setLevelID(const std::string& levelID){
	this->levelID = levelID;
}

void Level::setTileSet(GLuint tileSet){
	this->tileSet = tileSet;
}

void Level::setTileSize(float tileSize){
	this->tileSize = tileSize;
}

Tile* Level::getTile(unsigned layer, int gridX, int gridY){
	if (levelData.find(layer) == levelData.end()){
		return nullptr;
	}
	unsigned char tileIndex = levelData[layer][gridY][gridX];
	if (collisionData[tileSet].find(tileIndex) == collisionData[tileSet].end() || tileIndex == 0){
		return nullptr;
	}
	return collisionData[tileSet][tileIndex - 1];
}

float Level::getTileSize(){
	return tileSize;
}

float Level::getMapHeight(){
	return mapHeight * tileSize;
}

float Level::getMapLength(){
	return mapWidth * tileSize;
}

void Level::fillCollisionData(xml_node<>* tileNode){
	if (tileNode == nullptr){
		return;
	}
	static const char* collideCheck = "canCollide";
	static const char* trueCheck = "true";
	static const char* typeCheck = "tileType";
	unsigned tileIndex = stoi(tileNode->first_attribute("id")->value());
	if (collisionData[tileSet].find(tileIndex) != collisionData[tileSet].end()){
		return;
	}
	float x = 0, y = 0;
	float width = tileSize, height = tileSize;
	bool canCollide = false;
	TILE_TYPE tileType = SOLID;
	//Note: ALWAYS add properties in the tile properties, not in the collision edit dialogue box!
	xml_node<>* propertyNode = tileNode->first_node("properties");
	if (propertyNode == nullptr){
		throw "No properties for tilesheet!";
	}
	propertyNode = propertyNode->first_node("property");
	do{
		if (propertyNode == nullptr){
			break;
		}
		if (strcmp(collideCheck, propertyNode->first_attribute("name")->value()) == 0){
			canCollide = strcmp(trueCheck, propertyNode->first_attribute("value")->value()) == 0;
		}
		else if (strcmp(typeCheck, propertyNode->first_attribute("name")->value()) == 0){
			tileType = static_cast<TILE_TYPE>(stoi(propertyNode->first_attribute("value")->value()));
		}
		propertyNode = propertyNode->next_sibling("property");
	} while (propertyNode != nullptr);
	xml_node<>* objectNode = tileNode->first_node("objectgroup");
	if (objectNode != nullptr){
		if ((objectNode = objectNode->first_node("object")) != nullptr){
			if (objectNode->first_attribute("x") != nullptr){
				x = stof(objectNode->first_attribute("x")->value());
			}

			if (objectNode->first_attribute("y") != nullptr){
				y = stof(objectNode->first_attribute("y")->value());
			}

			if (objectNode->first_attribute("width") != nullptr){
				width = stof(objectNode->first_attribute("width")->value());
			}

			if (objectNode->first_attribute("height") != nullptr){
				width = stof(objectNode->first_attribute("height")->value());
			}
		}
	}
	if (collisionData[tileSet].find(tileIndex) != collisionData[tileSet].end()){
		throw "Repeated tile index!";
	}
	collisionData[tileSet][tileIndex] = new Tile();
	collisionData[tileSet][tileIndex]->height = height;
	collisionData[tileSet][tileIndex]->width = width;
	collisionData[tileSet][tileIndex]->x = x;
	collisionData[tileSet][tileIndex]->y = y;
	collisionData[tileSet][tileIndex]->canCollide = canCollide;
	collisionData[tileSet][tileIndex]->tileType = tileType;
	fillCollisionData(tileNode->next_sibling());
}

void Level::fillSpriteSheetData(xml_node<>* tileNode){
	xml_node<>* imageNode = tileNode->first_node("image");
	if (imageNode != nullptr){
		sheetHeight = stoi(imageNode->first_attribute("height")->value());
		sheetWidth = stoi(imageNode->first_attribute("width")->value());
		spriteCountX = sheetWidth / tileWidth;
		spriteCountY = sheetHeight / tileHeight;
	}
	if (tileNode->first_attribute("id") == nullptr){
		tileNode = tileNode->first_node("tile");
	}
	fillCollisionData(tileNode);
	fillTileTestData();
	fillVertexArrays();
}

void Level::setPlayerEntity(CompositeEntity* playerEntity){
	this->playerEntity = playerEntity;
}

void Level::setBackgroundColor(float r, float b, float g, float a){
	backgroundColor.r = r;
	backgroundColor.b = b;
	backgroundColor.g = g;
	backgroundColor.a = a;
}

CompositeEntity* Level::getPlayerEntity(){
	return playerEntity;
}

Color Level::getBackgroundColor(){
	return backgroundColor;
}

void Level::freeMemory(){

	playerEntity = nullptr;

	if (collisionData.size() != 0){
		for (std::unordered_map<GLuint, std::unordered_map<unsigned, Tile*>>::iterator itr = collisionData.begin(); itr != collisionData.end(); itr++){
			if (itr->second.size() != 0){
				for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
					if (itr2->second != nullptr){
						try{
							itr2->second->height;
							delete itr2->second;
							itr2->second = nullptr;
						}
						catch (char* e){
							itr2->second = nullptr;
						}
					}
					itr2 = itr->second.erase(itr2);
					if (itr2 == itr->second.end()){
						break;
					}
				}
			}
			itr = collisionData.erase(itr);
			if (itr == collisionData.end()){
				break;
			}
		}
	}
}

void Level::fillTileTestData(){
	for (std::unordered_map<unsigned, unsigned char**>::iterator itr = levelData.begin(); itr != levelData.end(); itr++){
		for (int y = 0; y < mapHeight; y++){
			levelDataTiles[itr->first].resize(mapHeight);
			for (int x = 0; x < mapWidth; x++){
				levelDataTiles[itr->first][y].push_back(getTile(itr->first, x, y));
			}
		}
	}
}