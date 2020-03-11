#include "scene/components/fanMeshRenderer.hpp"

#include "scene/components/fanTransform.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/ecs/fanECSConfig.hpp"
#include "core/math/shapes/fanAABB.hpp"
#include "core/math/shapes/fanConvexHull.hpp"
#include "core/time/fanProfiler.hpp"
#include "render/fanRenderSerializable.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanMesh.hpp"

// Editor
#include "core/imgui/fanModals.hpp"

namespace fan
{
	REGISTER_TYPE_INFO( MeshRenderer, TypeInfo::Flags::EDITOR_COMPONENT, "" )

		//================================================================================================================================
		//================================================================================================================================
		void MeshRenderer::OnAttach()
	{
		Component::OnAttach();

		m_mesh = m_gameobject->AddEcsComponent<ecsMesh>();
		m_mesh->Init();

/*		m_gameobject->GetScene().onRegisterMeshRenderer.Emmit( this );*/
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnDetach()
	{
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsMesh>();
/*		m_gameobject->GetScene().onUnRegisterMeshRenderer.Emmit( this );*/

		m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_AABB );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetMesh( Mesh* _mesh )
	{
		if ( _mesh != nullptr )
		{
			m_mesh->mesh = _mesh;
			m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_AABB );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetPath( std::string _path )
	{
		m_mesh->mesh.Init(_path);
		m_mesh->mesh.Resolve();
	}

	Mesh*		MeshRenderer::GetMesh()							 { return *m_mesh->mesh; }
	const Mesh* MeshRenderer::GetMesh() const					 { return *m_mesh->mesh; }
	int			MeshRenderer::GetRenderID() const				 { return m_mesh->renderID; }
	void		MeshRenderer::SetRenderID( const int _renderID ) { m_mesh->renderID = _renderID; }

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnGui()
	{
		Component::OnGui();

		ImGui::FanMeshPtr( "mesh", m_mesh->mesh );

		// Num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( GetMesh() != nullptr ? GetMesh()->GetIndices().size() / 3 : 0 );
		ImGui::Text( ss.str().c_str() );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool MeshRenderer::Load( const Json& _json )
	{
		std::string pathBuffer;

		Serializable::LoadMeshPtr( _json, "path", m_mesh->mesh );
		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool MeshRenderer::Save( Json& _json ) const
	{
		Component::Save( _json );

		Serializable::SaveMeshPtr( _json, "path", m_mesh->mesh );

		return true;
	}
}