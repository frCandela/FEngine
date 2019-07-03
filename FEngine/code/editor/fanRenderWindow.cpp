#include "fanIncludes.h"

#include "editor/fanRenderWindow.h"
#include "vulkan/pipelines/vkPostprocessPipeline.h"
#include "vulkan/vkRenderer.h"
#include "util/fanTime.h"

namespace editor {

	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow() {

	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::Draw() {
		if (IsVisible() == true ) {

			vk::Renderer &	renderer = vk::Renderer::GetRenderer();

			bool isVisible = IsVisible();
			if (ImGui::Begin("Rendering", &isVisible)) {
				if (ImGui::CollapsingHeader("Post-processing")) {
					vk::PostprocessPipeline::Uniforms uniforms = vk::Renderer::GetRenderer().GetPostprocessPipeline()->GetUniforms();

					// Filter color
					if (ImGui::ColorEdit3("Filter##1", &uniforms.color.r, util::Imgui::colorEditFlags)) {

						vk::Renderer::GetRenderer().GetPostprocessPipeline()->SetUniforms(uniforms);
					}
				}

				if (ImGui::CollapsingHeader("Global")) {
					float tmpFps = Time::GetFPS();
					if (ImGui::DragFloat("Framerate", &tmpFps, 1.f, Time::minFps, 144.f)) {
						Time::SetFPS(tmpFps);
					}

					// Clear color
					glm::vec4 clearColor = renderer.GetClearColor();
					if (ImGui::ColorEdit3("Clear color", &clearColor.r, util::Imgui::colorEditFlags)) {
						renderer.SetClearColor(clearColor);
					}
				}
			} ImGui::End();
			SetVisible(isVisible);
		}
	}
}