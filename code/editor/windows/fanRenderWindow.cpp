#include "editor/windows/fanRenderWindow.hpp"

#include "editor/fanModals.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletons/fanRenderResources.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow( Renderer& _renderer )
		: EditorWindow( "renderer", ImGui::IconType::RENDERER16 )
		, m_renderer( _renderer )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::OnGui( EcsWorld& _world )
	{
		SCOPED_PROFILE( render );

		const RenderResources& renderResources = _world.GetSingleton<RenderResources>();

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( "Renderer" );

		if ( ImGui::CollapsingHeader( "Loaded meshes : " ) )
		{
			for ( Mesh * mesh : renderResources.mMeshManager->GetMeshes() )
			{
				ImGui::Text("ref: %d name: %s", mesh->GetRefCount(), mesh->mPath.c_str() );
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
			for ( const auto pair : Prefab::s_resourceManager.GetList() )
			{
				ImGui::Text( "ref: %d name: %s", pair.second->GetRefCount(), pair.second->GetPath().c_str() );
			}
		}

		if ( ImGui::CollapsingHeader( "Rendered Mesh : " ) )
		{
			const std::vector<DrawData>& meshArray = m_renderer.mDrawModels.mDrawData;
			for ( uint32_t meshIndex = 0; meshIndex < meshArray.size(); meshIndex++ )
			{
				const DrawData& drawData = meshArray[ meshIndex ];
				if ( drawData.mesh != nullptr )
				{
					ImGui::Text( drawData.mesh->mPath.c_str() );
				}
				else
				{
					ImGui::Text( "Empty slot" );
				}
			}
		}

		UniformLights& lights = m_renderer.mDrawModels.mUniforms.mUniformsLights;
		if ( ImGui::CollapsingHeader( "Directional lights : " ) )
		{
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.dirLightsNum; lightIndex++ )
			{
				UniformDirectionalLight light = lights.dirLights[ lightIndex ];
				ImGui::DragFloat3( "dir ", &light.direction[ 0 ] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[ 0 ], ImGui::fanColorEditFlags );
			}
			ImGui::PopItemWidth();
		}
		if ( ImGui::CollapsingHeader( "Point lights : " ) )
		{
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.pointLightNum; lightIndex++ )
			{
				UniformPointLight& light = lights.pointlights[ lightIndex ];
				ImGui::DragFloat3( "pos ##pos", &light.position[ 0 ] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[ 0 ], ImGui::fanColorEditFlags );
			}
			ImGui::PopItemWidth();
		}
	}
}