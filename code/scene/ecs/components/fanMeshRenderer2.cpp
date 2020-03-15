#include "scene/ecs/components/fanMeshRenderer2.hpp"

#include "core/fanISerializable.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( MeshRenderer2, "mesh_renderer" );

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::MESH_RENDERER16;
		_info.onGui = &MeshRenderer2::OnGui;
		_info.init = &MeshRenderer2::Init;
		_info.load = &MeshRenderer2::Load;
		_info.save = &MeshRenderer2::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer2::Init( ecComponent& _component )
	{
		MeshRenderer2& meshRenderer = static_cast<MeshRenderer2&>( _component );
		meshRenderer.mesh = nullptr;
		meshRenderer.renderID = -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer2::OnGui( ecComponent& _meshRenderer )
	{
		MeshRenderer2& meshRenderer = static_cast<MeshRenderer2&>( _meshRenderer );
		ImGui::FanMeshPtr( "mesh", meshRenderer.mesh );

		// num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( *meshRenderer.mesh != nullptr ? meshRenderer.mesh->GetIndices().size() / 3 : 0 );
		ImGui::Text( ss.str().c_str() );

	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer2::Save( const ecComponent& _meshRenderer, Json& _json )
	{
		const MeshRenderer2& meshRenderer = static_cast<const MeshRenderer2&>( _meshRenderer );
		Serializable::SaveMeshPtr( _json, "path", meshRenderer.mesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer2::Load( ecComponent& _meshRenderer, const Json& _json )
	{
		MeshRenderer2& meshRenderer = static_cast<MeshRenderer2&>( _meshRenderer );
		Serializable::LoadMeshPtr( _json, "path", meshRenderer.mesh );
	}
}