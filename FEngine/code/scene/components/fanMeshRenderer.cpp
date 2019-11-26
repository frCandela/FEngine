#include "fanGlobalIncludes.h"
#include "scene/components/fanMeshRenderer.h"

#include "renderer/fanRenderer.h"
#include "renderer/fanMesh.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "scene/fanRessourcePtr.h"
#include "core/math/shapes/fanAABB.h"
#include "core/math/shapes/fanConvexHull.h"
#include "core/files/fanFbxImporter.h"
#include "ecs/fanECSConfig.h"
#include "core/time/fanProfiler.h"

// Editor
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_TYPE_INFO(MeshRenderer, TypeInfo::Flags::EDITOR_VISIBLE)

	Signal< MeshRenderer * >				MeshRenderer::onRegisterMeshRenderer;
	Signal< MeshRenderer * >				MeshRenderer::onUnRegisterMeshRenderer;

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnAttach() {
		Component::OnAttach();

		ecsMesh ** tmpMesh = &const_cast<ecsMesh*>( m_mesh );
		*tmpMesh = m_gameobject->AddEcsComponent<ecsMesh>();
		m_mesh->Init();

		onRegisterMeshRenderer.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsMesh>();
		onUnRegisterMeshRenderer.Emmit(this);

		m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_AABB );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetMesh( Mesh * _mesh )
	{
		m_mesh->mesh = MeshPtr( _mesh, _mesh->GetPath() );
		if( _mesh != nullptr && ! _mesh->GetIndices().empty() ) {	
			m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_AABB );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetPath( std::string _path ) {	
		m_mesh->mesh.InitUnresolved(_path);
	}

	Mesh *			MeshRenderer::GetMesh() { return *m_mesh->mesh; }
	const Mesh *	MeshRenderer::GetMesh() const { return *m_mesh->mesh; }

	int		MeshRenderer::GetRenderID() const { return m_mesh->renderID; }
	void	MeshRenderer::SetRenderID( const int _renderID ) { m_mesh->renderID = _renderID; }

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnGui() {
		Component::OnGui();

		ImGui::FanMesh( "mesh", &m_mesh->mesh );

		// Num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( GetMesh() != nullptr ?   GetMesh()->GetIndices().size() / 3  :  0 );		
		ImGui::Text(ss.str().c_str());
	}

	//================================================================================================================================
	//================================================================================================================================
	bool MeshRenderer::Load( const Json & _json ) {
		std::string pathBuffer;

		LoadMeshPtr( _json, "path", m_mesh->mesh );
		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool MeshRenderer::Save( Json & _json ) const {
		
		Component::Save( _json );

		SaveMeshPtr( _json, "path", m_mesh->mesh );
		
		return true;
	}
}