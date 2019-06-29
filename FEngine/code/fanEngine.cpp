#include "fanIncludes.h"

#include "fanEngine.h"
#include "vulkan/vkRenderer.h"
#include "util/fanTime.h"
#include "util/fanInput.h"
#include "editor/fanMainMenuBar.h"
#include "editor/fanRenderWindow.h"	

namespace fan {
	Engine * Engine::ms_engine = nullptr;

	Engine::Engine() :
		m_applicationShouldExit(false) {
		ms_engine = this;

		m_mainMenuBar = new editor::MainMenuBar();
		m_renderWindow = new editor::RenderWindow();
	}

	void Engine::Exit() {
		m_applicationShouldExit = true;
		std::cout << "Exit application" << std::endl;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run()
	{
		vk::Renderer renderer({ 1280,720 });

		float lastUpdateTime = Time::ElapsedSinceStartup();
		while (renderer.WindowIsOpen() == true && m_applicationShouldExit == false)
		{
			const float time = Time::ElapsedSinceStartup();
			const float updateDelta = time - lastUpdateTime;

			if (updateDelta > 1.f / Time::GetFPS()) {
				Input::NewFrame();
				lastUpdateTime = time;

				DrawUI();

				renderer.DrawFrame();
			}
		}
	}

	void Engine::DrawUI() {
		m_mainMenuBar->Draw();

		if (m_renderWindow->IsVisible()) {
			m_renderWindow->Draw();
		}
	}
}