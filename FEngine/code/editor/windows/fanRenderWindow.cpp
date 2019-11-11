#include "fanGlobalIncludes.h"

#include "editor/windows/fanRenderWindow.h"
#include "editor/fanModals.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanMesh.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "core/time/fanTime.h"
#include "core/time/fanProfiler.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow( Renderer * _renderer ) :
		EditorWindow("renderer", ImGui::IconType::RENDERER )
		, m_renderer( _renderer )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::OnGui() {
		SCOPED_PROFILE( render )

		RessourceManager * ressourceManager = m_renderer->GetRessourceManager();

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text("Renderer");

		// Display mesh list
		if ( ImGui::CollapsingHeader( "Loaded meshes : " ) ) {
			for (Mesh * mesh : m_renderer->GetRessourceManager()->GetMeshList() ){
				ImGui::Text( mesh->GetPath().c_str() );
			}
		}
		// display textures list
		const std::vector< Texture * > & textures = ressourceManager->GetTextures();
		if ( ImGui::CollapsingHeader( "Loaded textures : " ) ) {
			for ( int textureIndex = 0; textureIndex < textures.size(); textureIndex++ ) {
				const Texture * texture = textures[textureIndex];
				std::stringstream ss;
				ss << texture->GetSize().x << " " << texture->GetSize().x << "\t" << texture->GetPath();
				ImGui::Text( ss.str().c_str() );
			}
		}

		if ( ImGui::CollapsingHeader( "Rendered Mesh : " ) ) {
			const std::vector<DrawData> & meshArray = m_renderer->GetMeshArray();
			for ( uint32_t meshIndex = 0; meshIndex < meshArray.size(); meshIndex++ ) {
				const DrawData & drawData = meshArray[meshIndex];
				if ( drawData.mesh != nullptr ) {
					ImGui::Text( drawData.mesh->GetPath().c_str() );
				} else {
					ImGui::Text( "Empty slot" );
				}
			}
		}

		LightsUniforms & lights = m_renderer->GetForwardPipeline()->m_lightUniforms;
		if ( ImGui::CollapsingHeader( "Directional lights : " ) ) {
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.dirLightsNum; lightIndex++ ) {
				DirectionalLightUniform light  = lights.dirLights[lightIndex];
				ImGui::DragFloat3("dir ", &light.direction[0] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[0], ImGui::fanColorEditFlags );
			}
			ImGui::PopItemWidth();
		}
		if ( ImGui::CollapsingHeader( "Point lights : " ) ) {
			ImGui::PushItemWidth( 150 );
			for ( size_t lightIndex = 0; lightIndex < lights.pointLightNum; lightIndex++ ) {
				PointLightUniform& light = lights.pointlights[lightIndex];
				ImGui::DragFloat3( "pos ##pos", &light.position[0] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[0], ImGui::fanColorEditFlags );				
			}
			ImGui::PopItemWidth();
		}
	}
}