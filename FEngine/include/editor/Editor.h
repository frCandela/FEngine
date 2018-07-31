#pragma once

#include "GameObject.h"

#include <vector>
#include <utility>
#include <array>

#include "glm/glm.hpp"

struct Triangle
{
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;

	// Möller–Trumbore intersection algorithm
	bool RayCast(glm::vec3 origin, glm::vec3 dir, glm::vec3* outIntersection)
	{
		float EPSILON = 0.0000001f;
		glm::vec3 e1 = v1 - v0;	// edge 1
		glm::vec3 e2 = v2 - v0;	// edge 2
		glm::vec3 h = glm::cross(dir, e2);	// 
		float a = dot(e1, h);
		if (a > -EPSILON && a < EPSILON)	// d colinear to the e1 e2 plane
			return false;

		float f = 1 / a;
		glm::vec3 s = origin - v0;
		float u = f * dot(s, h);
		if (u < 0.0 || u > 1.0)
			return false;

		glm::vec3 q = cross(s, e1);
		float v = f * glm::dot(dir, q);
		if (v < 0.0 || u + v > 1.0)
			return false;

		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * dot(e2, q);
		if (t > EPSILON) // ray intersection
		{
			*outIntersection = origin + dir * t;
			return true;
		}
			
		// This means that there is a line intersection but not a ray intersection.
		return false;
	}
};

struct Cube
{
	std::array< Triangle, 12 > triangles;
	glm::vec3 position;
	float halfSize;
		 
	Cube(float halfSize = 0.5f, glm::vec3 position = {0,0,0}) :
		position(position)
		,halfSize(halfSize)
	{		
		SetCube();
	}

	void SetPosition(glm::vec3 pos = { 0,0,0 })
	{
		if (position != pos)
		{
			position = pos;
			SetCube();
		}
	}

private:
	void SetCube()
	{
		const glm::vec3 e1 = position + glm::vec3( -halfSize,-halfSize,-halfSize );
		const glm::vec3 e2 = position + glm::vec3( -halfSize,-halfSize,halfSize );
		const glm::vec3 e3 = position + glm::vec3( -halfSize,halfSize,-halfSize );
		const glm::vec3 e4 = position + glm::vec3( -halfSize,halfSize,halfSize );
		const glm::vec3 e5 = position + glm::vec3( halfSize,-halfSize,-halfSize );
		const glm::vec3 e6 = position + glm::vec3( halfSize,-halfSize,halfSize );
		const glm::vec3 e7 = position + glm::vec3( halfSize,halfSize,-halfSize );
		const glm::vec3 e8 = position + glm::vec3( halfSize,halfSize,halfSize );

		triangles[0] = { e1,e2,e4 };
		triangles[1] = { e1,e3,e4 };
		triangles[2] = { e5,e6,e8 };
		triangles[3] = { e5,e7,e8 };
		triangles[4] = { e2,e6,e8 };
		triangles[5] = { e2,e4,e8 };
		triangles[6] = { e1,e5,e7 };
		triangles[7] = { e1,e3,e7 };
		triangles[8] = { e3,e4,e8 };
		triangles[9] = { e3,e7,e8 };
		triangles[10] = { e1,e2,e6 };
		triangles[11] = { e1,e5,e6 };
	}
	
};



struct GameobjectEditorData
{

};

// Creates the editing fonctionalities of the engine
class Editor
{
public:
	Editor()
	{

	}

	~Editor()
	{

	}

private:



	std::vector< std::pair<GameObject *, GameobjectEditorData > > m_gameObjects;


};
