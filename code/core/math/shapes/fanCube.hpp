#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/math/shapes/fanTriangle.hpp"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	struct Cube : public Shape
	{
		Cube(const btVector3 _position, const float _halfSize);

		void SetPosition(const btVector3 _pos);

		const		std::array< Triangle, 12 >& GetTriangles() const { return m_triangles; }
		float		GetHalfSize() const { return m_halfSize; }
		btVector3	GetPosition() const { return m_position; }

		virtual bool RayCast(const btVector3 _origin, const btVector3 _direction, btVector3& outIntersection) const override;

	private:
		std::array< Triangle, 12 > m_triangles;
		btVector3 m_position;
		float m_halfSize;

		void InitCube();
	};
}
