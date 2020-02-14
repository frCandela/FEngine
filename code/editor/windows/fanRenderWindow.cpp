#include "editor/windows/fanRenderWindow.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/imgui/fanModals.hpp"
#include "core/time/fanTime.hpp"
#include "render/pipelines/fanPostprocessPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/pipelines/fanForwardPipeline.hpp"
#include "render/fanResourceManager.hpp"
#include "render/core/fanFrameBuffer.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow() :
		EditorWindow( "renderer", ImGui::IconType::RENDERER16 )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::OnGui()
	{
		SCOPED_PROFILE( render )

			ResourceManager& resourceManager = ResourceManager::Get();

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( "Renderer" );

		// Display mesh list
		if ( ImGui::CollapsingHeader( "Loaded meshes : " ) )
		{
			for ( auto pair : Mesh::s_resourceManager.GetList() )
			{
				ImGui::Text("ref: %d name: %s", pair.second->GetRefCount(), pair.second->GetPath().c_str() );
			}
		}
		// display textures list
		const std::vector< Texture* >& textures = resourceManager.GetTextures();
		if ( ImGui::CollapsingHeader( "Loaded textures : " ) )
		{
			for ( int textureIndex = 0; textureIndex < textures.size(); textureIndex++ )
			{
				const Texture* texture = textures[ textureIndex ];
				std::stringstream ss;
				ss << texture->GetSize().x << " " << texture->GetSize().x << "\t" << texture->GetPath();
				ImGui::Text( ss.str().c_str() );
			}
		}

		if ( ImGui::CollapsingHeader( "Rendered Mesh : " ) )
		{
			const std::vector<DrawData>& meshArray = m_renderer->GetMeshArray();
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

		LightsUniforms& lights = m_renderer->GetForwardPipeline()->m_lightUniforms;
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

			const VkExtent2D gameExtent = m_renderer->GetGameFrameBuffers()->GetExtent();
			const VkExtent2D postprocessExtent = m_renderer->GetPostProcessFramebuffers()->GetExtent();
			const VkExtent2D swapchainExtent = m_renderer->GetSwapchainFramebuffers()->GetExtent();

			ImGui::Text( "%d x %d game", gameExtent.width, gameExtent.height );
			ImGui::Text( "%d x %d postprocess", postprocessExtent.width, postprocessExtent.height );
			ImGui::Text( "%d x %d swapchain", swapchainExtent.width, swapchainExtent.height );

			ImGui::PopItemWidth();
		}
	}
}