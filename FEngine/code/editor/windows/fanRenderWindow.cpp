#include "fanGlobalIncludes.h"

#include "editor/windows/fanRenderWindow.h"
#include "editor/fanModals.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanMesh.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"
#include "core/fanTime.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow( Renderer * _renderer ) :
		EditorWindow("Renderer")
		, m_renderer( _renderer )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::OnGui() {
		if (ImGui::CollapsingHeader("Post-processing")) {
			// Filter color
			glm::vec4& color = m_renderer->GetPostprocessPipeline()->uniforms.color;
			if (ImGui::ColorEdit3("Filter##1", &color[0], gui::colorEditFlags)) {
				m_renderer->GetPostprocessPipeline()->UpdateUniformBuffers();
			}
		}

		if (ImGui::CollapsingHeader("Global")) {
			// Clear color
			glm::vec4 clearColor = m_renderer->GetClearColor();
			if (ImGui::ColorEdit3("Clear color", &clearColor.r, gui::colorEditFlags)) {
				m_renderer->SetClearColor(clearColor);
			}
		}

		RessourceManager * ressourceManager = m_renderer->GetRessourceManager();
		// Display mesh list
		if ( ImGui::CollapsingHeader( "Loaded meshes : " ) ) {
			for ( auto meshData : m_renderer->GetRessourceManager()->GetMeshData() ) {
				ImGui::Text( meshData.second->GetPath().c_str() );
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

		if ( ImGui::CollapsingHeader( "Rendered Models : " ) ) {
			const std::array< Mesh *, GlobalValues::s_maximumNumModels > & meshArray = m_renderer->GetMeshArray();
			for ( uint32_t meshIndex = 0; meshIndex < m_renderer->GetNumMesh(); meshIndex++ ) {
				const Mesh * mesh = meshArray[meshIndex];
				if ( mesh != nullptr ) {
					ImGui::Text( mesh->GetPath().c_str() );
				} else {
					ImGui::Text( "Empty slot" );
				}
			}
		}
	}
}