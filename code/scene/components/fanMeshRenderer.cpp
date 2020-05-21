#include "scene/components/fanMeshRenderer.hpp"

#include <sstream>
#include "core/fanSerializable.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::MESH_RENDERER16;
		_info.onGui = &MeshRenderer::OnGui;
		_info.init = &MeshRenderer::Init;
		_info.load = &MeshRenderer::Load;
		_info.save = &MeshRenderer::Save;
		_info.editorPath = "/";
		_info.name = "mesh renderer";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::Init( EcsWorld& _world, EcsComponent& _component )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		meshRenderer.mesh = nullptr;
		meshRenderer.renderID = -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		ImGui::FanMeshPtr( "mesh", meshRenderer.mesh );

		// num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( *meshRenderer.mesh != nullptr ? meshRenderer.mesh->GetIndices().size() / 3 : 0 );
		ImGui::Text( ss.str().c_str() );

	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::Save( const EcsComponent& _component, Json& _json )
	{
		const MeshRenderer& meshRenderer = static_cast<const MeshRenderer&>( _component );
		Serializable::SaveMeshPtr( _json, "path", meshRenderer.mesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::Load( EcsComponent& _component, const Json& _json )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		Serializable::LoadMeshPtr( _json, "path", meshRenderer.mesh );
	}
}