#include "fanIncludes.h"

#include "core/ressources/fanMesh.h"
#include "core/files/fanFbxImporter.h"

namespace ressource {
	REGISTER_TYPE_INFO(Mesh)
	util::Signal< Mesh * > Mesh::onMeshLoad;
	const char * Mesh::defaultMeshPath = "content/_default/mesh.fbx";

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
			if (importer.GetMesh(*this)) {
				onMeshLoad.Emmit(this);
 			}
		}
		Ressource::Load();
	}

}