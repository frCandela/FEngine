#pragma once

#include "engine/singletons/fanEngineResources.hpp"
#include "editor/fanGuiInfos.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanTexture.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "core/resources/fanResourceManager.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEngineResources
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Renderer16;
            info.mGroup      = EngineGroups::Scene;
            info.mEditorName = "engine resources";
            info.onGui       = &GuiEngineResources::OnGui;
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            EngineResources& engineResources = static_cast<EngineResources&>( _singleton );

            if( ImGui::CollapsingHeader( "prefabs" ) )
            {
                std::vector<Prefab*> prefabs;
                engineResources.mResourceManager->Get<Prefab>( prefabs );
                for( Prefab* prefab : prefabs )
                {
                    ImGui::Text( prefab->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "meshes" ) )
            {
                const std::vector<Mesh*>& meshes = engineResources.mMeshManager->GetMeshes();
                for( const Mesh         * mesh : meshes )
                {
                    ImGui::Text( mesh->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "meshes2D" ) )
            {
                const std::vector<Mesh2D*>& meshes = engineResources.mMesh2DManager->GetMeshes();
                for( const Mesh2D         * mesh : meshes )
                {
                    ImGui::Text( mesh->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "textures" ) )
            {
                std::vector<Texture*> textures;
                engineResources.mResourceManager->Get<Texture>( textures );
                for( const Texture         * texture : textures )
                {
                    ImGui::Text( texture->mPath.c_str() );
                }
            }
        }
    };
}
