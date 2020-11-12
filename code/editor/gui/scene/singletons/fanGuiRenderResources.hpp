#pragma once

#include "scene/singletons/fanRenderResources.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanTexture.hpp"

namespace fan
{
    struct GuiRenderResources
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Renderer16;
            info.mGroup = EngineGroups::SceneRender;
            info.mEditorName  = "render resources";
            info.onGui  = &GuiRenderResources::OnGui;
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            RenderResources& renderResources = static_cast<RenderResources&>( _singleton );
            if( ImGui::CollapsingHeader( "meshes" ) )
            {
                const std::vector<Mesh*>& meshes = renderResources.mMeshManager->GetMeshes();
                for( const Mesh         * mesh : meshes )
                {
                    ImGui::Text( mesh->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "meshes2D" ) )
            {
                const std::vector<Mesh2D*>& meshes = renderResources.mMesh2DManager->GetMeshes();
                for( const Mesh2D         * mesh : meshes )
                {
                    ImGui::Text( mesh->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "textures" ) )
            {
                const std::vector<Texture*>& textures = renderResources.mTextureManager->GetTextures();
                for( const Texture         * texture : textures )
                {
                    ImGui::Text( texture->mPath.c_str() );
                }
            }
        }
    };
}
