#include "fanGlobalIncludes.h"
#include "core/math/shapes/fanConvexHull.h"

#include "quickhull/QuickHull.hpp"
#include "bullet/LinearMath/btConvexHull.h"

namespace fan {
	namespace shape
	{	
		//================================================================================================================================
		//================================================================================================================================
		void ConvexHull::ComputeBulletHull(const std::vector<btVector3>& _pointCloud) {
			Clear();

			// Calculates hull
			HullResult	hull;
			HullDesc	desc(QF_TRIANGLES, (int)_pointCloud.size(), _pointCloud.data());
			HullLibrary lib;
			if (lib.CreateConvexHull(desc, hull) == QE_FAIL) {
				Debug::Warning("unable to compute convex hull");
			}
			else {
				// Copy indices
				m_indices.reserve(hull.mNumIndices);
				for (unsigned indexIndex = 0; indexIndex < hull.mNumIndices; indexIndex++) {
					m_indices.push_back((uint32_t)hull.m_Indices[indexIndex]);
				}

				// Copy vertices
				m_vertices.reserve(hull.mNumOutputVertices);
				for (unsigned vertexIndex = 0; vertexIndex < hull.mNumOutputVertices; vertexIndex++) {
					m_vertices.push_back(hull.m_OutputVertices[vertexIndex]);
				}
			}

			lib.ReleaseResult(hull);
		}

		//================================================================================================================================
		//================================================================================================================================
		void ConvexHull::ComputeQuickHull(const std::vector<btVector3>& _pointCloud) {
			Clear();

			// Convert data to quickhull vectices
			std::vector<quickhull::Vector3<float>> pointCloud;
			pointCloud.reserve(_pointCloud.size());
			for (int point = 0; point < _pointCloud.size(); point++) {
				const btVector3 & vertex = _pointCloud[point];
				pointCloud.push_back({ vertex[0],vertex[1],vertex[2] });
			}

			// Calculates hull
			quickhull::QuickHull<float> qh;
			quickhull::ConvexHull<float> hull;
			hull = qh.getConvexHull(pointCloud, true, false);

			// Copy indices
			const std::vector<size_t>& indexBuffer = hull.getIndexBuffer();
			m_indices.reserve(indexBuffer.size());
			assert(indexBuffer.size() % 3 == 0);
			for (int indexIndex = 0; indexIndex < indexBuffer.size() / 3 ; indexIndex++){
				m_indices.push_back((uint32_t)indexBuffer[3 * indexIndex + 0]);
				m_indices.push_back((uint32_t)indexBuffer[3 * indexIndex + 2]);
				m_indices.push_back((uint32_t)indexBuffer[3 * indexIndex + 1]);
			}

			// Copy vertices
			const quickhull::VertexDataSource<float>&	vertexBuffer = hull.getVertexBuffer();
			m_vertices.reserve(vertexBuffer.size());
			for (int vertexIndex = 0; vertexIndex < vertexBuffer.size(); vertexIndex++) {
				quickhull::Vector3<float> vertex = vertexBuffer[vertexIndex];
				m_vertices.push_back(btVector3(vertex.x, vertex.y, vertex.z));
			}
		}

		void ConvexHull::Clear() {
			m_vertices.clear();
			m_indices.clear();
		}
	}

}