#pragma once

namespace fan {
	namespace shape
	{
		//================================================================================================================================
		// Converts a points cloud to a convex hull
		// ComputeQuickHull is always faster than ComputeBulletHull ( up x300 times faster ).
		// ComputeBulletHull gives more accurate results with less triangles
		//================================================================================================================================
		class ConvexHull {
		public:
			ConvexHull() {}

			void ComputeBulletHull(const std::vector<btVector3>& _pointCloud);
			void ComputeQuickHull(const std::vector<btVector3>& _pointCloud);
			void Clear();

			const std::vector<btVector3> & GetVertices() const { return m_vertices;  }
			const std::vector<uint32_t> & GetIndices() const   { return m_indices; }

		private:
			std::vector<btVector3> m_vertices;
			std::vector<uint32_t>  m_indices;
		};
	}
}