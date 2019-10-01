#include "fanGlobalIncludes.h"

#include "renderer/fanMesh.h"
#include "core/files/fanFbxImporter.h"
#include "core/math/shapes/fanConvexHull.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanRenderer.h"

namespace fan {
	REGISTER_TYPE_INFO(Mesh)

	//================================================================================================================================
	//================================================================================================================================
	Mesh::Mesh(const std::string& _path) :
	m_path ( _path )
	,m_vertices(0)
	,m_indices(0)
	,m_vertexBuffer(nullptr)
	,m_indexBuffer(nullptr) {

	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh::~Mesh() {
		delete m_indexBuffer;
		delete m_vertexBuffer;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::Load( ) {	
		Ressource::Load();
		SetRessourceID(DSID(m_path.c_str()));
 		FBXImporter importer;
 		if (importer.LoadScene(m_path) == true) {
			if (importer.GetMesh(*this) == false ) {
				Debug::Get() << "Failed to load mesh : " << m_path << Debug::Endl();
			}
			else {
				OptimizeVertices();
				GenerateConvexHull();
			}
		}
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
	//================================================================================================================================
	void Mesh::GenerateConvexHull() {
		if (m_convexHull == nullptr) {
			m_convexHull = new ConvexHull();
		}

		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve(m_vertices.size());
		for (int point = 0; point < m_vertices.size(); point++) {
			Vertex & vertex = m_vertices[point];
			pointCloud.push_back(btVector3(vertex.pos.x, vertex.pos.y, vertex.pos.z));
		}

		m_convexHull->ComputeQuickHull(pointCloud);
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Mesh::GenerateBuffers( Device & _device ) {

		delete(m_indexBuffer );
		delete( m_vertexBuffer );
		m_indexBuffer = new Buffer( _device );
		m_vertexBuffer = new Buffer( _device );

		{
			const VkDeviceSize size = sizeof( m_indices[0] ) * m_indices.size();
			m_indexBuffer->Create(
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
			stagingBuffer.CopyBufferTo( cmd, m_indexBuffer->GetBuffer(), size );
			_device.EndSingleTimeCommands( cmd );
		}
		{
			const VkDeviceSize size = sizeof( m_vertices[0] ) * m_vertices.size();
			m_vertexBuffer->Create(
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
			stagingBuffer2.CopyBufferTo( cmd2, m_vertexBuffer->GetBuffer(), size );
			_device.EndSingleTimeCommands( cmd2 );
		}
	}
}