#include "fanGlobalIncludes.h"

#include "editor/windows/fanRenderWindow.h"
#include "editor/fanModals.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanRenderer.h"
#include "core/fanTime.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow() :
		EditorWindow("render")
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::OnGui() {
		Renderer &	renderer = Renderer::Get();

		if (ImGui::CollapsingHeader("Post-processing")) {
			// Filter color
			glm::vec4& color = Renderer::Get().GetPostprocessPipeline()->uniforms.color;
			if (ImGui::ColorEdit3("Filter##1", &color[0], gui::colorEditFlags)) {
				Renderer::Get().GetPostprocessPipeline()->UpdateUniformBuffers();
			}
		}

		if (ImGui::CollapsingHeader("Global")) {
			float tmpFps = Time::GetFPS();
			if (ImGui::DragFloat("Framerate", &tmpFps, 1.f, Time::GetMinFPS(), Time::GetMaxFPS())) {
				Time::SetFPS(tmpFps);
			}

			// Clear color
			glm::vec4 clearColor = renderer.GetClearColor();
			if (ImGui::ColorEdit3("Clear color", &clearColor.r, gui::colorEditFlags)) {
				renderer.SetClearColor(clearColor);
			}
		}
	}
}