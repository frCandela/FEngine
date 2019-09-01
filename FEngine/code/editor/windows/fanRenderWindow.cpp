#include "fanIncludes.h"

#include "editor/windows/fanRenderWindow.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanRenderer.h"
#include "core/fanTime.h"
#include "editor/fanModals.h"

namespace fan
{
	namespace editor {

		//================================================================================================================================
		//================================================================================================================================
		RenderWindow::RenderWindow() {

		}

		//================================================================================================================================
		//================================================================================================================================
		void RenderWindow::Draw() {
			if (IsVisible() == true) {

				Renderer &	renderer = Renderer::GetRenderer();

				bool isVisible = IsVisible();
				if (ImGui::Begin("Rendering", &isVisible)) {
					if (ImGui::CollapsingHeader("Post-processing")) {
						vk::PostprocessPipeline::Uniforms uniforms = Renderer::GetRenderer().GetPostprocessPipeline()->GetUniforms();

						// Filter color
						if (ImGui::ColorEdit3("Filter##1", &uniforms.color.r, gui::colorEditFlags)) {

							Renderer::GetRenderer().GetPostprocessPipeline()->SetUniforms(uniforms);
						}
					}

					if (ImGui::CollapsingHeader("Forward rendering")) {
						vk::ForwardPipeline::FragUniforms uniforms = Renderer::GetRenderer().GetForwardPipeline()->GetFragUniforms();
						bool uniformsModified = false;
						if (ImGui::SliderFloat("Ambiant light", &uniforms.ambiantIntensity, 0.f, 1.f)) {
							uniformsModified = true;
						}
						if (ImGui::SliderInt("specular hardness", &uniforms.specularHardness, 0, 128)) {
							uniformsModified = true;
						}
						if (ImGui::SliderFloat3("lightColor", &uniforms.lightColor.x, 0.f, 1.f)) {
							uniformsModified = true;
						}
						if (ImGui::DragFloat3("lightPos", &uniforms.lightPos.x)) {
							uniformsModified = true;
						}

						if (uniformsModified == true) {
							Renderer::GetRenderer().GetForwardPipeline()->SetFragUniforms(uniforms);
						}

					}

					if (ImGui::CollapsingHeader("Global")) {
						float tmpFps = fan::Time::GetFPS();
						if (ImGui::DragFloat("Framerate", &tmpFps, 1.f, fan::Time::minFps, 144.f)) {
							fan::Time::SetFPS(tmpFps);
						}

						// Clear color
						glm::vec4 clearColor = renderer.GetClearColor();
						if (ImGui::ColorEdit3("Clear color", &clearColor.r, gui::colorEditFlags)) {
							renderer.SetClearColor(clearColor);
						}
					}
				} ImGui::End();
				SetVisible(isVisible);
			}
		}
	}
}