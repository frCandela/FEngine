#pragma once

#include "core/shapes/fanConvexHull.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResource.hpp"
#include "render/fanVertex.hpp"
#include "render/fanMeshManager.hpp"
#include "render/core/fanBuffer.hpp"

namespace fan
{

	struct Device;
	class MeshManager;

	//================================================================================================================================
	// 3D mesh composed of triangles
	// can have a convex hull computed for it
	//================================================================================================================================
	class Mesh : public Resource 
	{
	public:
		static MeshManager s_resourceManager;

		Mesh();
		~Mesh();

		bool RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const;

		std::vector<Vertex>& GetVertices() { return m_vertices; }
		const std::vector<Vertex>& GetVertices() const { return m_vertices; }
		std::vector<uint32_t>& GetIndices() { return m_indices; }
		const std::vector<uint32_t>& GetIndices() const { return m_indices; }
		Buffer& GetIndexBuffer() { return m_indexBuffer[ m_currentBuffer ]; }
		Buffer& GetVertexBuffer() { return m_vertexBuffer[ m_currentBuffer ]; }
		const ConvexHull& GetHull() { return m_convexHull; }
		std::string						GetPath() const { return m_path; }

		// Useful when the mesh changes very often
		void SetHostVisible( const bool _hostVisible ) { m_hostVisible = _hostVisible; }
		void SetOptimizeVertices( const bool _optimizeVertices ) { m_optimizeVertices = _optimizeVertices; }
		void SetAutoUpdateHull( const bool _autoUpdateHull ) { m_autoUpdateHull = _autoUpdateHull; }

		void GenerateGpuBuffers( Device& _device );
		void DeleteGpuBuffers( Device& _device );

		bool LoadFromFile( const std::string& _path );
		bool LoadFromVertices( const std::vector<Vertex>& _vertices );
		//DECLARE_TYPE_INFO(Mesh, void )
	private:
		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;

		std::string		m_path;

		bool m_hostVisible = false;
		bool m_optimizeVertices = true;
		bool m_autoUpdateHull = true;

		uint32_t m_currentBuffer = 0;
		Buffer m_indexBuffer[ 3 ];
		Buffer m_vertexBuffer[ 3 ];
		ConvexHull m_convexHull;

		void OptimizeVertices();
		void GenerateConvexHull();
	};
}