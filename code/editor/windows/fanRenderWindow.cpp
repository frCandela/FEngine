#include "editor/windows/fanRenderWindow.hpp"

#include "editor/fanModals.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "scene/singletons/fanSceneResources.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/fanPrefabManager.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	RenderWindow::RenderWindow( Renderer& _renderer )
		: EditorWindow( "renderer", ImGui::IconType::Renderer16 )
		, mRenderer( _renderer )
	{}

	//========================================================================================================
	//========================================================================================================
	void RenderWindow::OnGui( EcsWorld& _world )
	{
		SCOPED_PROFILE( render );

		const RenderResources& renderResources = _world.GetSingleton<RenderResources>();

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( "Renderer" );

		if ( ImGui::CollapsingHeader( "Loaded 3D meshes : " ) )
		{
			for ( Mesh * mesh : renderResources.mMeshManager->GetMeshes() )
			{
				ImGui::Text("ref: %d name: %s", mesh->GetRefCount(), mesh->mPath.c_str() );
			}
		}

        if ( ImGui::CollapsingHeader( "Loaded 2D meshes : " ) )
        {
            for ( Mesh2D * mesh : renderResources.mMesh2DManager->GetMeshes() )
            {
                ImGui::Text("name: %s", mesh->mPath.c_str() );
            }
        }

		if ( ImGui::CollapsingHeader( "Loaded textures : " ) )
		{
			for ( Texture * tex : renderResources.mTextureManager->GetTextures() )
			{
                ImGui::Text( "ref: %d size: %u x %u name: %s",
                             tex->GetRefCount(),
                             tex->mExtent.width,
                             tex->mExtent.height,
                             tex->mPath.c_str() );
            }
		}

		// Display mesh list
		if ( ImGui::CollapsingHeader( "Loaded prefabs : " ) )
		{
		    SceneResources& sceneResources = _world.GetSingleton<SceneResources>();
			for ( const auto pair : sceneResources.mPrefabManager->GetPrefabs() )
			{
				ImGui::Text( "ref: %d name: %s", pair.second->GetRefCount(), pair.second->mPath.c_str() );
			}
		}

		if ( ImGui::CollapsingHeader( "Rendered Mesh : " ) )
		{
			const std::vector<DrawData>& meshArray = mRenderer.mDrawModels.mDrawData;
			for ( uint32_t meshIndex = 0; meshIndex < meshArray.size(); meshIndex++ )
			{
				const DrawData& drawData = meshArray[ meshIndex ];
				if ( drawData.mMesh != nullptr )
				{
					ImGui::Text( drawData.mMesh->mPath.c_str() );
				}
				else
				{
					ImGui::Text( "Empty slot" );
				}
			}
		}

		UniformLights& lights = mRenderer.mDrawModels.mUniforms.mUniformsLights;
		if ( ImGui::CollapsingHeader( "Directional lights : " ) )
		{
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.mDirLightsNum; lightIndex++ )
			{
				UniformDirectionalLight light = lights.mDirLights[ lightIndex ];
				ImGui::DragFloat3( "dir ", &light.mDirection[ 0 ] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.mDiffuse[ 0 ], ImGui::fanColorEditFlags );
			}
			ImGui::PopItemWidth();
		}
		if ( ImGui::CollapsingHeader( "Point lights : " ) )
		{
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.mPointLightNum; lightIndex++ )
			{
				UniformPointLight& light = lights.mPointlights[ lightIndex ];
				ImGui::DragFloat3( "pos ##pos", &light.mPosition[ 0 ] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.mDiffuse[ 0 ], ImGui::fanColorEditFlags );
			}
			ImGui::PopItemWidth();
		}
	}
}