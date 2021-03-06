#include <editor/fanResourceInfos.hpp>
#include "editor/windows/fanRenderWindow.hpp"

#include "editor/fanModals.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "core/resources/fanResources.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "editor/fanDragnDrop.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        RenderWindow& renderWindow = static_cast<RenderWindow&>( _singleton );
        renderWindow.mRenderer = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiRenderWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        RenderWindow& renderWindow = static_cast<RenderWindow&>( _singleton );

        SCOPED_PROFILE( render );

        const Application& app = _world.GetSingleton<Application>();

        ImGui::Icon( ImGui::Renderer16, { 16, 16 } );
        ImGui::SameLine();
        ImGui::Text( "Renderer" );

        if( ImGui::CollapsingHeader( "Loaded 3D meshes : " ) )
        {
            std::vector<ResourcePtr<Mesh>> meshes;
            app.mResources->Get<Mesh>( meshes );
            for( ResourcePtr<Mesh> mesh : meshes )
            {
                ImGui::Text( "name: %s", mesh->mPath.c_str() );
                ImGui::FanBeginDragDropSourceResource( mesh, Mesh::Info::sType, ImGuiDragDropFlags_SourceAllowNullID );
            }
        }

        if( ImGui::CollapsingHeader( "Loaded 2D meshes : " ) )
        {
            std::vector<ResourcePtr<Mesh2D>> meshes2D;
            app.mResources->Get<Mesh2D>( meshes2D );
            for( ResourcePtr<Mesh2D> mesh : meshes2D )
            {
                ImGui::Text( "name: %s", mesh->mPath.c_str() );
            }
        }

        if( ImGui::CollapsingHeader( "Loaded textures : " ) )
        {
            std::vector<ResourcePtr<Texture>> textures;
            app.mResources->Get<Texture>( textures );
            for( ResourcePtr<Texture> tex : textures )
            {
                ImGui::Text( "ref: %d size: %u x %u name: %s", tex.mData.mHandle->mRefCount, tex->mExtent.width, tex->mExtent.height, tex->mPath.c_str() );
                ImGui::FanBeginDragDropSourceResource( tex, Texture::Info::sType, ImGuiDragDropFlags_SourceAllowNullID );
            }
        }

        if( ImGui::CollapsingHeader( "Loaded fonts : " ) )
        {
            std::vector<ResourcePtr<Font>> fonts;
            app.mResources->Get<Font>( fonts );
            for( fan::ResourcePtr<fan::Font> font : fonts )
            {
                ImGui::Text( font->mPath.c_str() );
                ImGui::FanBeginDragDropSourceResource( font, Mesh::Info::sType, ImGuiDragDropFlags_SourceAllowNullID );
            }
        }

        // Display mesh list
        if( ImGui::CollapsingHeader( "Loaded prefabs : " ) )
        {
            std::vector<ResourcePtr<Prefab>> prefabs;
            app.mResources->Get<Prefab>( prefabs );
            for( const ResourcePtr<Prefab> prefab : prefabs )
            {
                ImGui::Text( "ref: %d name: %s", prefab.mData.mHandle->mRefCount, prefab->mPath.c_str() );
            }
        }

        UniformLights& lights = renderWindow.mRenderer->mDrawModels.mUniforms.mUniformsLights;
        if( ImGui::CollapsingHeader( "Directional lights : " ) )
        {
            ImGui::PushItemWidth( 150 );
            for( size_t lightIndex = 0; lightIndex < lights.mDirLightsNum; lightIndex++ )
            {
                UniformDirectionalLight light = lights.mDirLights[lightIndex];
                ImGui::DragFloat3( "dir ", &light.mDirection[0] );
                ImGui::SameLine();
                ImGui::ColorEdit3( "diffuse", &light.mDiffuse[0], ImGui::fanColorEditFlags );
            }
            ImGui::PopItemWidth();
        }
        if( ImGui::CollapsingHeader( "Point lights : " ) )
        {
            ImGui::PushItemWidth( 150 );
            for( size_t lightIndex = 0; lightIndex < lights.mPointLightNum; lightIndex++ )
            {
                UniformPointLight& light = lights.mPointlights[lightIndex];
                ImGui::DragFloat3( "pos ##pos", &light.mPosition[0] );
                ImGui::SameLine();
                ImGui::ColorEdit3( "diffuse", &light.mDiffuse[0], ImGui::fanColorEditFlags );
            }
            ImGui::PopItemWidth();
        }
    }
}