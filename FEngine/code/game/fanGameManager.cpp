#include "fanGlobalIncludes.h"
#include "game/fanGameManager.h"

#include "fanEngine.h"
#include "core/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"
#include "scene/components/fanCamera.h"
#include "renderer/fanRenderer.h"


namespace fan {
	namespace game {

		REGISTER_EDITOR_COMPONENT(GameManager);
		REGISTER_TYPE_INFO(GameManager)
		
		//================================================================================================================================
		//================================================================================================================================
		void GameManager::Start() {
			Debug::Log("Start satellite");
			m_camera = GetEntity()->GetComponent<scene::Camera>();
			if (m_camera == nullptr) {
				Debug::Warning("Game manager has no camera attached");
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void GameManager::Update(const float /*_delta*/) {
			ImGui::Begin("Game manager"); {
				if ( ImGui::Button("Switch cameras (tab)") || Keyboard::IsKeyPressed(GLFW_KEY_TAB)) {
					Debug::Log("switching camera");
					scene::Camera * editorCamera = Engine::GetEngine().GetEditorCamera();
					scene::Camera * currentCamera = Renderer::Get().GetMainCamera();

					if (currentCamera == editorCamera) {
						Renderer::Get().SetMainCamera(m_camera);
					} else {
						Renderer::Get().SetMainCamera(editorCamera);
					}
				}
			} ImGui::End();
		}

		//================================================================================================================================
		//================================================================================================================================
		void GameManager::OnGui() {
			Actor::OnGui();
		}

		//================================================================================================================================
		//================================================================================================================================
		bool GameManager::Load(std::istream& /*_in*/) {
// 			if (!ReadSegmentHeader(_in, "radius:")) { return false; }
// 			if (!ReadFloat(_in, m_radius)) { return false; }
			return true; 
		}

		//================================================================================================================================
		//================================================================================================================================
		bool GameManager::Save(std::ostream& /*_out*/, const int /*_indentLevel*/) const {
// 			const std::string indentation = GetIndentation(_indentLevel);
// 			_out << indentation << "radius: " << m_radius << std::endl;
			return true; 
		}
	}
}