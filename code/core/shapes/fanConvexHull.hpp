#pragma once

#include "core/shapes/fanShape.hpp"
#include <vector>
#include <cstdint>

namespace fan
{
	//================================================================================================================================
	// Converts a points cloud to a convex hull
	// ComputeQuickHull is always faster than ComputeBulletHull ( up to x300 times faster ).
	// ComputeBulletHull gives more accurate results with less triangles
	//================================================================================================================================
	class ConvexHull : public Shape
	{
	public:
		ConvexHull() {}

		void ComputeBulletHull( const std::vector<btVector3>& _pointCloud );
		void ComputeQuickHull( const std::vector<btVector3>& _pointCloud );
		void Clear();

		bool RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const override;
		bool IsEmpty() const { return m_indices.size() == 0; }

		const std::vector<btVector3>& GetVertices() const { return m_vertices; }
		const std::vector<uint32_t>& GetIndices() const { return m_indices; }

	private:
		std::vector<btVector3> m_vertices;
		std::vector<uint32_t>  m_indices;
	};
}
