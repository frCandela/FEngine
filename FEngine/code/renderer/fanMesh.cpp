#include "fanGlobalIncludes.h"

#include "renderer/fanMesh.h"
#include "core/files/fanFbxImporter.h"
#include "core/math/shapes/fanConvexHull.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanRenderer.h"

namespace fan {
	REGISTER_TYPE_INFO(Mesh)

	Signal< Mesh* > Mesh::s_onMeshLoad;
	Signal< > Mesh::s_onMeshDelete;

	//================================================================================================================================
	//================================================================================================================================
	Mesh::Mesh(const std::string& _path) :
		m_path ( _path )
		, m_vertices( 0 )
		, m_indices( 0 )
		, m_vertexBuffer { nullptr ,nullptr ,nullptr }
		, m_indexBuffer { nullptr ,nullptr ,nullptr }
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh::~Mesh() {
		s_onMeshDelete.Emmit();
		for (int bufferIndex = 0; bufferIndex < 3 ; bufferIndex++) {
			delete m_indexBuffer[bufferIndex];
			delete m_vertexBuffer[bufferIndex];
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh::Load( ) {
 		FBXImporter importer;
 		if (importer.LoadScene(m_path) == true) {
			if (importer.GetMesh(*this) != false ) {
				OptimizeVertices();
				s_onMeshLoad.Emmit(this);
				return true;
			}
		}
		Debug::Get() << "Failed to load mesh : " << m_path << Debug::Endl();
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Mesh::LoadFromVertices( const std::vector<Vertex>&	_vertices ) {
		m_vertices = _vertices;

		// Generate fake indices
		m_indices.clear();
		m_indices.reserve(_vertices.size());
		for (int vertIndex = 0; vertIndex < _vertices.size(); vertIndex++) {
			m_indices.push_back( vertIndex );
		}

		// Cleanup
		OptimizeVertices();
		s_onMeshLoad.Emmit( this );
		return true;
	}

	
	//================================================================================================================================
	// Removes duplicates vertices & generates a corresponding index buffer
	//================================================================================================================================
	void Mesh::OptimizeVertices() {

		std::unordered_map<Vertex, uint32_t> verticesMap = {};

		std::vector<Vertex> uniqueVertices;
		std::vector<uint32_t>	uniqueIndices;

		for (int indexIndex = 0; indexIndex < m_indices.size(); indexIndex++) {
			Vertex vertex = m_vertices[m_indices[indexIndex]];
			auto it = verticesMap.find(vertex);
			if (it == verticesMap.end()) {
				verticesMap[vertex] = static_cast<uint32_t>(uniqueVertices.size());
				uniqueIndices.push_back(static_cast<uint32_t>(uniqueVertices.size()));
				uniqueVertices.push_back(vertex);				
			} else {
				uniqueIndices.push_back(it->second);
			}
		}
		m_vertices = uniqueVertices;
		m_indices = uniqueIndices;
	} 

	//================================================================================================================================
	// Creates a convex hull from the mesh geometry
	//================================================================================================================================
	void  Mesh::GenerateConvexHull( ConvexHull & _outConvexHull ) {
		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve(m_vertices.size());
		for (int point = 0; point < m_vertices.size(); point++) {
			Vertex & vertex = m_vertices[point];
			pointCloud.push_back(btVector3(vertex.pos.x, vertex.pos.y, vertex.pos.z));
		}
		_outConvexHull.ComputeQuickHull(pointCloud);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::GenerateBuffers( Device & _device ) {

		if( m_indices.empty() ) {
			return;
		}

		m_currentBuffer = (m_currentBuffer + 1) % 3;

		delete( m_indexBuffer[m_currentBuffer] );
		delete( m_vertexBuffer[m_currentBuffer] );
		m_indexBuffer[m_currentBuffer] = new Buffer( _device );
		m_vertexBuffer[m_currentBuffer] = new Buffer( _device );

		{
			const VkDeviceSize size = sizeof( m_indices[0] ) * m_indices.size();
			m_indexBuffer[m_currentBuffer]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			Buffer stagingBuffer( _device );
			stagingBuffer.Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer.SetData( m_indices.data(), size );
			VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
			stagingBuffer.CopyBufferTo( cmd, m_indexBuffer[m_currentBuffer]->GetBuffer(), size );
			_device.EndSingleTimeCommands( cmd );
		}
		{
			const VkDeviceSize size = sizeof( m_vertices[0] ) * m_vertices.size();
			m_vertexBuffer[m_currentBuffer]->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			Buffer stagingBuffer2( _device );
			stagingBuffer2.Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer2.SetData( m_vertices.data(), size );
			VkCommandBuffer cmd2 = _device.BeginSingleTimeCommands();
			stagingBuffer2.CopyBufferTo( cmd2, m_vertexBuffer[m_currentBuffer]->GetBuffer(), size );
			_device.EndSingleTimeCommands( cmd2 );
		}
	}
}