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
#include "engine/singletons/fanEngineResources.hpp"
#include "engine/singletons/fanEngineResources.hpp"
#include "core/resources/fanResourceManager.hpp"
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

        const EngineResources& engineResources = _world.GetSingleton<EngineResources>();

        ImGui::Icon( ImGui::Renderer16, { 16, 16 } );
        ImGui::SameLine();
        ImGui::Text( "Renderer" );

        if( ImGui::CollapsingHeader( "Loaded 3D meshes : " ) )
        {
            for( Mesh* mesh : engineResources.mMeshManager->GetMeshes() )
            {
                ImGui::Text( "index: %d name: %s", mesh->mIndex, mesh->mPath.c_str() );
                ImGui::FanBeginDragDropSourceMesh( mesh, ImGuiDragDropFlags_SourceAllowNullID );
            }
        }

        if( ImGui::CollapsingHeader( "Loaded 2D meshes : " ) )
        {
            for( Mesh2D* mesh : engineResources.mMesh2DManager->GetMeshes() )
            {
                ImGui::Text( "name: %s", mesh->mPath.c_str() );
            }
        }

        if( ImGui::CollapsingHeader( "Loaded textures : " ) )
        {
            std::vector<Texture*> textures;
            engineResources.mResourceManager.Get<Texture>( textures );
            for( Texture* tex : textures )
            {
                ImGui::Text( "ref: %d size: %u x %u name: %s", tex->mRefCount, tex->mExtent.width, tex->mExtent.height, tex->mPath.c_str() );
                ImGui::FanBeginDragDropSourceTexture( tex, ImGuiDragDropFlags_SourceAllowNullID );
            }
        }

        if( ImGui::CollapsingHeader( "Loaded fonts : " ) )
        {
            std::vector<Font*> fonts;
            engineResources.mResourceManager->Get<Font>( fonts );
            for( Font* font : fonts )
            {
                ImGui::Text( font->mPath.c_str() );
                ImGui::FanBeginDragDropSourceFont( font, ImGuiDragDropFlags_SourceAllowNullID );
            }
        }

        // Display mesh list
        if( ImGui::CollapsingHeader( "Loaded prefabs : " ) )
        {
            std::vector<Prefab*> prefabs;
            engineResources.mResourceManager->Get<Prefab>( prefabs );
            for( const Prefab* prefab : prefabs )
            {
                ImGui::Text( "ref: %d name: %s", prefab->mRefCount, prefab->mPath.c_str() );
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