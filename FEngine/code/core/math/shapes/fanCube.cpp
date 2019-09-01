#include "fanIncludes.h"

#include "core/math/shapes/fanCube.h"

namespace fan {
	namespace shape
	{
		//================================================================================================================================
		//================================================================================================================================
		Cube::Cube(const btVector3 _position, float const _halfSize) :
			m_position(_position)
			, m_halfSize(_halfSize) {
			InitCube();
		}

		//================================================================================================================================
		//================================================================================================================================
		void Cube::SetPosition(const btVector3 _pos)
		{
			if (m_position != _pos)
			{
				m_position = _pos;
				InitCube();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Cube::RayCast(const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection) const
		{
			btVector3 intersection;
			float closestDistance = std::numeric_limits<float>::max();
			for (const Triangle& triangle : m_triangles)
			{
				if (triangle.RayCast(_origin, _direction, intersection))
				{
					float distance = intersection.distance(_origin);
					if (distance < closestDistance)
					{
						closestDistance = distance;
						_outIntersection = intersection;
					}
				}
			}
			return closestDistance != std::numeric_limits<float>::max();
		}

		//================================================================================================================================
		//================================================================================================================================
		void Cube::InitCube()
		{
			const btVector3 e1 = m_position + btVector3(-m_halfSize, -m_halfSize, -m_halfSize);
			const btVector3 e2 = m_position + btVector3(-m_halfSize, -m_halfSize, m_halfSize);
			const btVector3 e3 = m_position + btVector3(-m_halfSize, m_halfSize, -m_halfSize);
			const btVector3 e4 = m_position + btVector3(-m_halfSize, m_halfSize, m_halfSize);
			const btVector3 e5 = m_position + btVector3(m_halfSize, -m_halfSize, -m_halfSize);
			const btVector3 e6 = m_position + btVector3(m_halfSize, -m_halfSize, m_halfSize);
			const btVector3 e7 = m_position + btVector3(m_halfSize, m_halfSize, -m_halfSize);
			const btVector3 e8 = m_position + btVector3(m_halfSize, m_halfSize, m_halfSize);

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
	}
}
