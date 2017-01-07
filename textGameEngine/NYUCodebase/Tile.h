#ifndef TILE_H
#define TILE_H
#include "Vector3.h"
#include <vector>
enum TILE_TYPE {SOLID, SOLID_GRASS, WATER, CHECKPOINT, HAZARD, HAZARD_SPIKE_HORIZONTAL, HAZARD_SPIKE_VERTICAL, HAZARD_FIRE, SOLID_FLOOR, BACKGROUND_TILE, SOLID_PLATFORM};
struct Tile{
	unsigned tileID;
	TILE_TYPE tileType;
	float x, y;
	float width, height;
	bool canCollide;

	//Assumption: tile is a square!
	inline const std::vector<Vector3> getSATCoordinates(float tileSize, int tileY, int tileX){
		std::vector<Vector3> tileSATCoordinates;
		Vector3 uR, uL, lL, lR;
		uR.x = tileSize / 2;
		uR.y = -tileSize / 2;
		uR.z = 1;

		uL.x = -tileSize / 2;
		uL.y = -tileSize / 2;
		uL.z = 1;

		lL.x = -tileSize / 2;
		lL.y = tileSize / 2;
		lL.z = 1;
		
		lR.x = tileSize / 2;
		lR.y = tileSize / 2;
		lR.z = 1;



		uR.x += tileX * tileSize;
		uR.x -= (tileSize/2 - width/2);
		uR.y -= tileY * tileSize;
		uR.y -= y;
		
		uL.x += tileX * tileSize;
		uL.x += x;
		uL.y -= tileY * tileSize;
		uL.y -= y;

		lL.x += tileX * tileSize;
		lL.x += x;
		lL.y -= tileY * tileSize;
		lL.y += (tileSize / 2 - height / 2);

		lR.x += tileX * tileSize;
		lR.x -= (tileSize / 2 - width / 2);
		lR.y -= tileY * tileSize;
		lR.y += (tileSize / 2 - height / 2);

		tileSATCoordinates = { uR, uL, lL, lR };
		return tileSATCoordinates;
	}
};

#endif