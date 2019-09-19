#include "fanGlobalIncludes.h"

#include "renderer/fanMesh.h"
#include "core/files/fanFbxImporter.h"
#include "core/math/shapes/fanConvexHull.h"

namespace fan {
	REGISTER_TYPE_INFO(Mesh)

	//================================================================================================================================
	//================================================================================================================================
	Mesh::Mesh(const std::string& _path) :
	m_path ( _path )
	,m_vertices(0)
	,m_indices(0){

	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::Load( ) {	
		Ressource::Load();
		SetRessourceID(DSID(m_path.c_str()));
 		fan::FBXImporter importer;
 		if (importer.LoadScene(m_path) == true) {
			if (importer.GetMesh(*this) == false ) {
				fan::Debug::Get() << "Failed to load mesh : " << m_path << Debug::Endl();
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

		std::unordered_map<vk::Vertex, uint32_t> verticesMap = {};

		std::vector<vk::Vertex> uniqueVertices;
		std::vector<uint32_t>	uniqueIndices;

		for (int indexIndex = 0; indexIndex < m_indices.size(); indexIndex++) {
			vk::Vertex vertex = m_vertices[m_indices[indexIndex]];
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
			m_convexHull = new shape::ConvexHull();
		}

		// Generate points clouds from vertex list
		std::vector < btVector3> pointCloud;
		pointCloud.reserve(m_vertices.size());
		for (int point = 0; point < m_vertices.size(); point++) {
			vk::Vertex & vertex = m_vertices[point];
			pointCloud.push_back(btVector3(vertex.pos.x, vertex.pos.y, vertex.pos.z));
		}

		m_convexHull->ComputeQuickHull(pointCloud);
	}

}