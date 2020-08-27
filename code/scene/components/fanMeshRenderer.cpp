#include "scene/components/fanMeshRenderer.hpp"
#include <sstream>
#include "core/fanSerializable.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::MESH_RENDERER16;
		_info.mGroup      = EngineGroups::SceneRender;
		_info.onGui       = &MeshRenderer::OnGui;
		_info.load        = &MeshRenderer::Load;
		_info.save        = &MeshRenderer::Save;
		_info.mEditorPath = "/";
		_info.mName       = "mesh renderer";
	}

	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		meshRenderer.mMesh    = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::Save( const EcsComponent& _component, Json& _json )
	{
		const MeshRenderer& meshRenderer = static_cast<const MeshRenderer&>( _component );
		Serializable::SaveMeshPtr( _json, "path", meshRenderer.mMesh );
	}

	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::Load( EcsComponent& _component, const Json& _json )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		Serializable::LoadMeshPtr( _json, "path", meshRenderer.mMesh );
	}

    //========================================================================================================
    //========================================================================================================
    void MeshRenderer::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
        ImGui::FanMeshPtr( "mesh", meshRenderer.mMesh );

        // num triangles
        std::stringstream ss;
        ss << "triangles: ";
        ss << ( *meshRenderer.mMesh != nullptr ? meshRenderer.mMesh->mIndices.size() / 3 : 0 );
        ImGui::Text( ss.str().c_str() );

    }
}