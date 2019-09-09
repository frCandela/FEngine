#include "fanGlobalIncludes.h"

#include "renderer/fanMesh.h"
#include "core/files/fanFbxImporter.h"

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
		SetRessourceID(DSID(m_path.c_str()));
 		fan::FBXImporter importer;
 		if (importer.LoadScene(m_path) == true) {
			if (importer.GetMesh(*this) == false ) {
				fan::Debug::Get() << "Failed to load mesh : " << m_path << Debug::Endl();
 			}
		}
		Ressource::Load();
	}

}