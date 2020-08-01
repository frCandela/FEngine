#include "fanRenderResources.hpp"

#include "render/fanMesh.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void RenderResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.icon = ImGui::RENDERER16;
        _info.group = EngineGroups::SceneRender;
        _info.name = "render resources";
        _info.onGui = &RenderResources::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RenderResources& renderResources = static_cast<RenderResources&>( _singleton );
        (void) renderResources;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderResources::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RenderResources& renderResources = static_cast<RenderResources&>( _singleton );

        ImGui::Indent(); ImGui::Indent();
        {
            const std::vector< Mesh * >& meshes = renderResources.mMeshManager->GetMeshes();
            for( const Mesh* mesh : meshes )
            {
                ImGui::Text( mesh->mPath.c_str() );
            }
        }
        ImGui::Unindent(); ImGui::Unindent();
    }
}
