#include "editor/windows/fanRenderWindow.hpp"

#include "editor/fanModals.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"
#include "scene/fanPrefab.hpp"
#include "render/pipelines/fanPostprocessPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanMesh.hpp"

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
	void RenderWindow::OnGui( EcsWorld& /*_world*/ )
	{
		SCOPED_PROFILE( render );

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( "Renderer" );

		// Display mesh list
		if ( ImGui::CollapsingHeader( "Loaded meshes : " ) )
		{
			for ( const auto pair : Mesh::s_resourceManager.GetList() )
			{
				ImGui::Text("ref: %d name: %s", pair.second->GetRefCount(), pair.second->GetPath().c_str() );
			}
		}

		// display textures list
		const std::vector< Texture* >& textures = Texture::s_resourceManager.GetList();
		if ( ImGui::CollapsingHeader( "Loaded textures : " ) )
		{
			for ( int textureIndex = 0; textureIndex < textures.size(); textureIndex++ )
			{
				const Texture* tex = textures[ textureIndex ];
				ImGui::Text( "ref: %d size: %u x %u name: %s", tex->GetRefCount(), tex->extent.width, tex->extent.height, tex->path.c_str() );
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
			const std::vector<DrawData>& meshArray = m_renderer.GetDrawData();
			for ( uint32_t meshIndex = 0; meshIndex < meshArray.size(); meshIndex++ )
			{
				const DrawData& drawData = meshArray[ meshIndex ];
				if ( drawData.mesh != nullptr )
				{
					ImGui::Text( drawData.mesh->GetPath().c_str() );
				}
				else
				{
					ImGui::Text( "Empty slot" );
				}
			}
		}

		LightsUniforms& lights = m_renderer.GetForwardPipeline()->m_lightUniforms;
		if ( ImGui::CollapsingHeader( "Directional lights : " ) )
		{
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.dirLightsNum; lightIndex++ )
			{
				DirectionalLightUniform light = lights.dirLights[ lightIndex ];
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
				PointLightUniform& light = lights.pointlights[ lightIndex ];
				ImGui::DragFloat3( "pos ##pos", &light.position[ 0 ] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[ 0 ], ImGui::fanColorEditFlags );
			}
			ImGui::PopItemWidth();
		}
		if ( ImGui::CollapsingHeader( "Framebuffers : " ) )
		{
			ImGui::PushItemWidth( 150 );

			const VkExtent2D gameExtent = m_renderer.GetGameFrameBuffers()->GetExtent();
			const VkExtent2D postprocessExtent = m_renderer.GetPostProcessFramebuffers()->GetExtent();
			const VkExtent2D swapchainExtent = m_renderer.GetSwapchainFramebuffers()->GetExtent();

			ImGui::Text( "%d x %d game", gameExtent.width, gameExtent.height );
			ImGui::Text( "%d x %d postprocess", postprocessExtent.width, postprocessExtent.height );
			ImGui::Text( "%d x %d swapchain", swapchainExtent.width, swapchainExtent.height );

			ImGui::PopItemWidth();
		}
	}
}