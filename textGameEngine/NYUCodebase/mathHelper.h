#ifndef _MATH_HELPER_H
#define _MATH_HELPER_H
#include <algorithm>
#include "enumHelper.h"
#include <unordered_map>

enum VERTEX_INDICES {UPPER_RIGHT, UPPER_LEFT, LOWER_LEFT, LOWER_RIGHT, VERTEX_COUNT};

inline float lerp(float v0, float v1, float t){
	return (1 - t)*v0 + t*v1;
}

inline float easeIn(float from, float to, float time){
	float tVal = time*time*time*time*time;
	return (1.0f - tVal)*from + tVal*to;
}

inline float easeOut(float from, float to, float time) {
	float oneMinusT = 1.0f - time; 
	float tVal = 1.0f - (oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT);
	return (1.0f - tVal)*from + tVal*to;
}

inline float randomRanged(float maximum){
	return (-maximum * 0.5) + (maximum * ((float)rand() / (float)RAND_MAX));
}


/*
-------------PRIMARY ASSUMPTION FOR SAT COLLISION---------------

ALL VECTORS FOR SQUARE COLLISIONS ARE FORMATTED AS SUCH:

{UPPER-RIGHT, UPPER-LEFT, LOWER-LEFT, LOWER-RIGHT}

*/

inline bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector3> &points1, const std::vector<Vector3> &points2) {
	float normalX = -edgeY;
	float normalY = edgeX;
	float len = sqrtf(normalX*normalX + normalY*normalY);
	normalX /= len;
	normalY /= len;

	std::vector<float> e1Projected;
	std::vector<float> e2Projected;

	for (int i = 0; i < points1.size(); i++) {
		e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
	}
	for (int i = 0; i < points2.size(); i++) {
		e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
	}

	std::sort(e1Projected.begin(), e1Projected.end());
	std::sort(e2Projected.begin(), e2Projected.end());

	float e1Min = e1Projected[0];
	float e1Max = e1Projected[e1Projected.size() - 1];
	float e2Min = e2Projected[0];
	float e2Max = e2Projected[e2Projected.size() - 1];
	float e1Width = fabs(e1Max - e1Min);
	float e2Width = fabs(e2Max - e2Min);
	float e1Center = e1Min + (e1Width / 2.0);
	float e2Center = e2Min + (e2Width / 2.0);
	float dist = fabs(e1Center - e2Center);
	float p = dist - ((e1Width + e2Width) / 2.0);

	if (p < 0) {
		return true;
	}
	return false;
}

inline bool checkSATCollision(const std::vector<Vector3> &e1Points, const std::vector<Vector3> &e2Points) {
	for (int i = 0; i < e1Points.size(); i++) {
		float edgeX, edgeY;

		if (i == e1Points.size() - 1) {
			edgeX = e1Points[0].x - e1Points[i].x;
			edgeY = e1Points[0].y - e1Points[i].y;
		}
		else {
			edgeX = e1Points[i + 1].x - e1Points[i].x;
			edgeY = e1Points[i + 1].y - e1Points[i].y;
		}

		bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
		if (!result) {
			return false;
		}
	}
	for (int i = 0; i < e2Points.size(); i++) {
		float edgeX, edgeY;

		if (i == e2Points.size() - 1) {
			edgeX = e2Points[0].x - e2Points[i].x;
			edgeY = e2Points[0].y - e2Points[i].y;
		}
		else {
			edgeX = e2Points[i + 1].x - e2Points[i].x;
			edgeY = e2Points[i + 1].y - e2Points[i].y;
		}
		bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
		if (!result) {
			return false;
		}
	}
	return true;
}

//inline bool testSATSeparationForEdgeDirectional(float edgeX, float edgeY, const std::vector<Vector3> &points1, const std::vector<Vector3> &points2, DIRECTION directon) {
//	float normalX = -edgeY;
//	float normalY = edgeX;
//	float len = sqrtf(normalX*normalX + normalY*normalY);
//	normalX /= len;
//	normalY /= len;
//
//	std::vector<float> e1Projected;
//	std::vector<float> e2Projected;
//
//	for (int i = 0; i < points1.size(); i++) {
//		e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
//	}
//	for (int i = 0; i < points2.size(); i++) {
//		e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
//	}
//
//	std::sort(e1Projected.begin(), e1Projected.end());
//	std::sort(e2Projected.begin(), e2Projected.end());
//
//	float e1Min = e1Projected[0];
//	float e1Max = e1Projected[e1Projected.size() - 1];
//	float e2Min = e2Projected[0];
//	float e2Max = e2Projected[e2Projected.size() - 1];
//	float e1Width = fabs(e1Max - e1Min);
//	float e2Width = fabs(e2Max - e2Min);
//	float e1Center = e1Min + (e1Width / 2.0);
//	float e2Center = e2Min + (e2Width / 2.0);
//	float dist = fabs(e1Center - e2Center);
//	float p = dist - ((e1Width + e2Width) / 2.0);
//
//	if (p < 0) {
//		return true;
//	}
//	return false;
//}

inline bool checkSATCollisionDirectional(const std::vector<Vector3> &e1Points, const std::vector<Vector3> &e2Points, Vector3 e1V, Vector3 e2V, DIRECTION direction) {
	for (int i = 0; i < e1Points.size(); i++) {
		float edgeX, edgeY;

		if (i == e1Points.size() - 1) {
			edgeX = e1Points[0].x - e1Points[i].x;
			edgeY = e1Points[0].y - e1Points[i].y;
		}
		else {
			edgeX = e1Points[i + 1].x - e1Points[i].x;
			edgeY = e1Points[i + 1].y - e1Points[i].y;
		}

		bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
		if (!result) {
			return false;
		}
	}
	for (int i = 0; i < e2Points.size(); i++) {
		float edgeX, edgeY;

		if (i == e2Points.size() - 1) {
			edgeX = e2Points[0].x - e2Points[i].x;
			edgeY = e2Points[0].y - e2Points[i].y;
		}
		else {
			edgeX = e2Points[i + 1].x - e2Points[i].x;
			edgeY = e2Points[i + 1].y - e2Points[i].y;
		}
		bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
		if (!result) {
			return false;
		}
	}
	float e1Direc = atan((e1V.x - e2V.x) / (e1V.y - e2V.y));
	//float e2Direc = atan(e2V.x / e2V.y);
	switch (direction){
	case ALL_DIRECTIONS:
		return true;
		break;
	case UP:
		return e1V.y - e2V.y > 0;
		break;
	case LEFT:
		return e1V.x - e2V.x < 0;
		return false;
		break;
	case RIGHT:
		return e1V.x - e2V.x > 0;
		break;
	case DOWN:
		return e1V.y - e2V.y < 0;
		break;
	default:
		throw "Unknown direction!";
		break;
	}
	//Vector3 e1Vertex1, e1Vertex2, e2Vertex1, e2Vertex2;
	//float edgeX, edgeY;
	//bool result;
	//switch (direction){
	//	//Vertex containment order: uR, uL, lL, lR
	//case ALL_DIRECTIONS:
	//	return checkSATCollision(e1Points, e2Points);
	//	break;
	//case UP:
	//	e1Vertex1 = e1Points[UPPER_RIGHT];
	//	e1Vertex2 = e1Points[UPPER_LEFT];
	//	e2Vertex1 = e2Points[LOWER_LEFT];
	//	e2Vertex2 = e2Points[LOWER_RIGHT];
	//	break;
	//case DOWN:
	//	e1Vertex1 = e1Points[LOWER_LEFT];
	//	e1Vertex2 = e1Points[LOWER_RIGHT];
	//	e2Vertex1 = e2Points[UPPER_RIGHT];
	//	e2Vertex2 = e2Points[UPPER_LEFT];
	//	break;
	//case LEFT:
	//	e1Vertex1 = e1Points[UPPER_LEFT];
	//	e1Vertex2 = e1Points[LOWER_LEFT];
	//	e2Vertex1 = e2Points[LOWER_RIGHT];
	//	e2Vertex2 = e2Points[UPPER_RIGHT];
	//	break;
	//case RIGHT:
	//	e1Vertex1 = e1Points[LOWER_RIGHT];
	//	e1Vertex2 = e1Points[UPPER_RIGHT];
	//	e2Vertex1 = e2Points[UPPER_LEFT];
	//	e2Vertex2 = e2Points[LOWER_LEFT];
	//	break;
	//case X:
	//	return (checkSATCollisionDirectional(e1Points, e2Points, LEFT) || checkSATCollisionDirectional(e1Points, e2Points, RIGHT));
	//	break;
	//case Y:
	//	return (checkSATCollisionDirectional(e1Points, e2Points, UP) || checkSATCollisionDirectional(e1Points, e2Points, DOWN));
	//	break;
	//default:
	//	throw "Unknown direction!";
	//	break;
	//}
	//edgeX = e1Vertex2.x - e1Vertex1.x;
	//edgeY = e1Vertex2.y - e1Vertex1.y;
	//result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points) && checkSATCollision(e1Points, e2Points);
	//if (!result){
	//	return false;
	//}

	//edgeX = e2Vertex2.x - e2Vertex1.x;
	//edgeY = e2Vertex2.y - e2Vertex1.y;
	//result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points) && checkSATCollision(e1Points, e2Points);
	//if (!result){
	//	return false;
	//}
	//return true;
}

inline float getSATPenetrationDirectional(const std::vector<Vector3> &e1Points, const std::vector<Vector3> &e2Points, DIRECTION direction){
	//Vertex storage order: UPPER_RIGHT, UPPER_LEFT, LOWER_LEFT, LOWER_RIGHT
	float penetration = 0;
	float tmpPenetration = 0;
	//Vector3 uR1 = e1Points[UPPER_RIGHT], uL1 = e1Points[UPPER_LEFT], lL1 = e1Points[LOWER_LEFT], lR1 = e1Points[LOWER_RIGHT];
	//Vector3 uR2 = e2Points[UPPER_RIGHT], uL2 = e2Points[UPPER_LEFT], lL2 = e2Points[LOWER_LEFT], lR2 = e2Points[LOWER_RIGHT];
	switch (direction){
	case UP:
		for (int i = UPPER_RIGHT; i <= UPPER_LEFT; i++){
			for (int j = LOWER_LEFT; j <= LOWER_RIGHT; j++){
				tmpPenetration = e1Points[i].y - e2Points[j].y + 0.00001;
				if (tmpPenetration > penetration){
					penetration = tmpPenetration;
				}
			}
		}
		break;
	case DOWN:
		for (int i = UPPER_RIGHT; i <= UPPER_LEFT; i++){
			for (int j = LOWER_LEFT; j <= LOWER_RIGHT; j++){
				tmpPenetration = e2Points[i].y - e1Points[j].y + 0.00001;
				if (tmpPenetration > penetration){
					penetration = tmpPenetration;
				}
			}
		}
		break;
	case LEFT:
		for (int i = UPPER_LEFT; i <= LOWER_LEFT; i++){
			for (int j = UPPER_RIGHT; j <= LOWER_RIGHT; j = j == UPPER_RIGHT ? LOWER_RIGHT : VERTEX_COUNT){
				tmpPenetration = e2Points[j].x - e1Points[i].x + 0.00001;
				if (tmpPenetration > penetration){
					penetration = tmpPenetration;
				}
			}
		}
		break;
	case RIGHT:
		for (int i = UPPER_RIGHT; i <= LOWER_RIGHT; i = i == UPPER_RIGHT ? LOWER_RIGHT : VERTEX_COUNT){
			for (int j = UPPER_LEFT; j <= LOWER_LEFT; j++){
				tmpPenetration = e1Points[i].x - e2Points[j].x + 0.00001;
				if (tmpPenetration > penetration){
					penetration = tmpPenetration;
				}
			}
		}
		break;
	default:
		throw "ONLY USE CARDINAL DIRECTIONS FOR GETTING PENETRATION!";
		break;
	}
	return penetration;
}

inline float hexToBase10(const std::string& hex){
	int value = 0;
	int i = hex.size() - 1;
	for (int pos = 0; pos < hex.size(); pos++){
		int modifier = 0;
		switch (hex[pos]){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			modifier = hex[pos] - 48;
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			modifier = hex[pos] - 87;
			break;
		default:
			throw "Uknown hex character!";
			break;
		}
		int inc = pow(16, i);
		value += modifier * pow(16, i);
		i--;
	}
	return value;
}

class CompositeEntity;
inline int getEntityLayers(std::unordered_map<std::string, std::unordered_map<unsigned, std::vector<CompositeEntity*>>>& gameEntityRenderingOrder, const std::string& level){
	int max = gameEntityRenderingOrder.size();
	for (auto itr = gameEntityRenderingOrder[level].begin(); itr != gameEntityRenderingOrder[level].end(); itr++){
		if (itr->first > max){
			max = itr->first;
		}
	}
	return max;
}
//
//float sizeXPos = abs(entity->getSizePositive().x * rC + entity->getSizePositive().y * rS);
//float sizeXNeg = abs(entity->getSizeNegative().x * rC - entity->getSizePositive().y * rS);
//float sizeY = abs(entity->getSizeNegative().y * rC - entity->getSizePositive().x * rS);

//inline float getPenetration(Vector3 e1Pos, Vector3 e1Size, float e1Rotation, Vector3 e2Pos, Vector3 e2Size, float e2Rotation, DIRECTION direction){
//	float rC1 = cos(e1Rotation), rS1 = sin(e1Rotation);
//	float rC2 = cos(e2Rotation), rS2 = sin(e2Rotation);
//	float e1PositiveX = abs(e1Size.x * rC1 + e1Size.y * rS1);
//	float e1NegativeX = abs(e1Size.x * rC1 - e1Size.y * rS1);
//	float e1PositiveY = abs(e1Size.y * rC1 + e1Size.x * rS1);
//	float e1NegativeY = abs(e1Size.y * rC1 - e1Size.x * rS1);
//	float e2PositiveX = abs(e2Size.x * rC2 + e2Size.y * rS2);
//	float e2NegativeX = abs(e2Size.x * rC2 - e2Size.y * rS2);
//	float e2PositiveY = abs(e2Size.y * rC2 + e2Size.x * rS2);
//	float e2NegativeY = abs(e2Size.y * rC2 - e2Size.x * rS2);
//	float penetration = 0;
//	switch (direction){
//	case UP:
//		penetration = (e1Pos.y + e1PositiveY) - (e2Pos.y - e2NegativeY);
//		break;
//	case DOWN:
//		penetration = (e2Pos.y + e2PositiveY) - (e1Pos.y - e1NegativeY);
//		break;
//	case LEFT:
//		penetration = (e2Pos.x + e2PositiveX) - (e1Pos.x - e1NegativeX);
//		break;
//	case RIGHT:
//		penetration = (e1Pos.x + e1PositiveX) - (e2Pos.x - e2NegativeX);
//		break;
//	}
//	return penetration;
//}

#endif
