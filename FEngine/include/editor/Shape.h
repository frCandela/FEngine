#pragma once

#include "GameObject.h"

#include <vector>
#include <utility>
#include <array>

#include "glm/glm.hpp"

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Triangle
{
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;

	// Möller–Trumbore intersection algorithm
	bool RayCast(glm::vec3 origin, glm::vec3 dir, glm::vec3* outIntersection) const
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
	Cube(glm::vec3 position = { 0,0,0 }, float halfSize = 0.5f) :
		m_position(position)
		,m_halfSize(halfSize)
	{		
		SetCube();
	}

	float GetHalfSize() const { return m_halfSize; }
	glm::vec3 GetPosition() const { return m_position; }
	void SetPosition(glm::vec3 pos = { 0,0,0 })
	{
		if (m_position != pos)
		{
			m_position = pos;
			SetCube();
		}
	}

	const std::array< Triangle, 12 >& GetTriangles() const { return m_triangles;}

private:
	std::array< Triangle, 12 > m_triangles;
	glm::vec3 m_position;
	float m_halfSize;

	void SetCube()
	{
		const glm::vec3 e1 = m_position + glm::vec3( -m_halfSize,-m_halfSize,-m_halfSize );
		const glm::vec3 e2 = m_position + glm::vec3( -m_halfSize,-m_halfSize,m_halfSize );
		const glm::vec3 e3 = m_position + glm::vec3( -m_halfSize,m_halfSize,-m_halfSize );
		const glm::vec3 e4 = m_position + glm::vec3( -m_halfSize,m_halfSize,m_halfSize );
		const glm::vec3 e5 = m_position + glm::vec3( m_halfSize,-m_halfSize,-m_halfSize );
		const glm::vec3 e6 = m_position + glm::vec3( m_halfSize,-m_halfSize,m_halfSize );
		const glm::vec3 e7 = m_position + glm::vec3( m_halfSize,m_halfSize,-m_halfSize );
		const glm::vec3 e8 = m_position + glm::vec3( m_halfSize,m_halfSize,m_halfSize );

		m_triangles[0] = { e1,e2,e4 };
		m_triangles[1] = { e1,e3,e4 };
		m_triangles[2] = { e5,e6,e8 };
		m_triangles[3] = { e5,e7,e8 };
		m_triangles[4] = { e2,e6,e8 };
		m_triangles[5] = { e2,e4,e8 };
		m_triangles[6] = { e1,e5,e7 };
		m_triangles[7] = { e1,e3,e7 };
		m_triangles[8] = { e3,e4,e8 };
		m_triangles[9] = { e3,e7,e8 };
		m_triangles[10] = { e1,e2,e6 };
		m_triangles[11] = { e1,e5,e6 };
	}	
};

