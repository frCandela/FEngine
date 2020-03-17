#include "scene/ecs/components/fanMeshRenderer.hpp"

#include "core/fanISerializable.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( MeshRenderer, "mesh_renderer" );

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::MESH_RENDERER16;
		_info.onGui = &MeshRenderer::OnGui;
		_info.init = &MeshRenderer::Init;
		_info.load = &MeshRenderer::Load;
		_info.save = &MeshRenderer::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::Init( Component& _component )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		meshRenderer.mesh = nullptr;
		meshRenderer.renderID = -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::OnGui( Component& _meshRenderer )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _meshRenderer );
		ImGui::FanMeshPtr( "mesh", meshRenderer.mesh );

		// num triangles
		std::stringstream ss;
		ss << "triangles: ";
		ss << ( *meshRenderer.mesh != nullptr ? meshRenderer.mesh->GetIndices().size() / 3 : 0 );
		ImGui::Text( ss.str().c_str() );

	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::Save( const Component& _meshRenderer, Json& _json )
	{
		const MeshRenderer& meshRenderer = static_cast<const MeshRenderer&>( _meshRenderer );
		Serializable::SaveMeshPtr( _json, "path", meshRenderer.mesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshRenderer::Load( Component& _meshRenderer, const Json& _json )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _meshRenderer );
		Serializable::LoadMeshPtr( _json, "path", meshRenderer.mesh );
	}
}