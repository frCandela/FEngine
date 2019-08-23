#include "fanIncludes.h"

#include "editor/windows/fanRenderWindow.h"
#include "vulkan/pipelines/vkPostprocessPipeline.h"
#include "vulkan/pipelines/vkForwardPipeline.h"
#include "vulkan/vkRenderer.h"
#include "core/fanTime.h"
#include "editor/fanModals.h"

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

				if (ImGui::CollapsingHeader("Forward rendering")) {
					vk::ForwardPipeline::FragUniforms uniforms = vk::Renderer::GetRenderer().GetForwardPipeline()->GetFragUniforms();
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
						vk::Renderer::GetRenderer().GetForwardPipeline()->SetFragUniforms(uniforms);
					}

				}

				if (ImGui::CollapsingHeader("Global")) {
					float tmpFps = fan::Time::GetFPS();
					if (ImGui::DragFloat("Framerate", &tmpFps, 1.f, fan::Time::minFps, 144.f)) {
						fan::Time::SetFPS(tmpFps);
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