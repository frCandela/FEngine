#include "fanIncludes.h"

#include "core/ressources/fanMesh.h"
#include "core/files/fanFbxImporter.h"

namespace ressource {
	REGISTER_ABSTRACT_TYPE_INFO(Mesh)

	//================================================================================================================================
	//================================================================================================================================
	Mesh::Mesh(const std::string& _path) :
	m_path ( _path )
	,m_vertices(0)
	,m_indices(0){

	}

	//================================================================================================================================
	//================================================================================================================================
	void Mesh::Load() {
		// 		fan::Engine::GetEngine().GetRenderer().RemoveMesh(this);

		

 		util::FBXImporter importer;
 		if (importer.LoadScene(m_path) == true) {
			if (importer.GetMesh(*this)) {
				//fan::Engine::GetEngine().GetRenderer().AddMesh(this);
 			}
		}

		Ressource::Load();
	}

}