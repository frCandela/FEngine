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
                std::vector<ResourcePtr<Prefab>> prefabs;
                engineResources.mResources->Get<Prefab>( prefabs );
                for( ResourcePtr<Prefab> prefab : prefabs )
                {
                    ImGui::Text( prefab->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "meshes" ) )
            {
                std::vector<ResourcePtr<Mesh>> meshes;
                engineResources.mResources->Get<Mesh>( meshes );
                for( ResourcePtr<Mesh> mesh : meshes )
                {
                    ImGui::Text( mesh->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "meshes2D" ) )
            {
                std::vector<ResourcePtr<Mesh2D>> meshes2D;
                engineResources.mResources->Get<Mesh2D>( meshes2D );
                for( ResourcePtr<Mesh2D> mesh : meshes2D )
                {
                    ImGui::Text( mesh->mPath.c_str() );
                }
            }

            if( ImGui::CollapsingHeader( "textures" ) )
            {
                std::vector<ResourcePtr<Texture>> textures;
                engineResources.mResources->Get<Texture>( textures );
                for( ResourcePtr<Texture> texture : textures )
                {
                    ImGui::Text( texture->mPath.c_str() );
                }
            }
        }
    };
}
