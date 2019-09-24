#include "fanGlobalIncludes.h"
#include "game/fanGameManager.h"

#include "fanEngine.h"
#include "core/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanScene.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"
#include "scene/components/fanCamera.h"
#include "editor/components/fanFPSCamera.h"
#include "game/fanSpaceShip.h"	
#include "renderer/fanRenderer.h"


namespace fan {
	REGISTER_EDITOR_COMPONENT(GameManager);
	REGISTER_TYPE_INFO(GameManager)

		//================================================================================================================================
		//================================================================================================================================
		void GameManager::Start() {
		Debug::Log("Start satellite");
		m_camera = GetEntity()->GetComponent<Camera>();
		if (m_camera == nullptr) {
			Debug::Warning("Game manager has no camera attached");
			SetEnabled(false);
		}

		m_spaceShip = GetEntity()->GetScene()->FindComponentOfType<SpaceShip>();
		if (m_spaceShip == nullptr) {
			Debug::Warning("GameManager::Start : No spaceShip found");
			SetEnabled(false);
		}
		else {
			m_spaceShip->SetEnabled(false);
		}

		m_editorCameraController = GetEntity()->GetScene()->FindComponentOfType<FPSCamera>();
		if (m_editorCameraController == nullptr) {
			Debug::Warning("GameManager::Start : No editor CameraController found");
			SetEnabled(false);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::SwitchCameras() {
		Debug::Log("switching camera");
		Camera * editorCamera = Engine::GetEngine().GetEditorCamera();
		Camera * currentCamera = Engine::GetEngine().GetMainCamera();

		if (currentCamera == editorCamera) {
			Engine::GetEngine().SetMainCamera(m_camera);
			m_spaceShip->SetEnabled(true);
			m_editorCameraController->SetEnabled(false);
		}
		else {
			Engine::GetEngine().SetMainCamera(editorCamera);
			m_spaceShip->SetEnabled(false);
			m_editorCameraController->SetEnabled(true);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::Update(const float /*_delta*/) {
		if (Keyboard::IsKeyPressed(GLFW_KEY_TAB)) {
			SwitchCameras();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnGui() {
		Actor::OnGui();
		ImGui::Text("Press tab to switch cameras");
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Load(std::istream& _in) {
		Actor::Load(_in);

		// 			if (!ReadSegmentHeader(_in, "radius:")) { return false; }
		// 			if (!ReadFloat(_in, m_radius)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Save(std::ostream& _out, const int _indentLevel) const {
		Actor::Save(_out, _indentLevel);
		// 			const std::string indentation = GetIndentation(_indentLevel);
		// 			_out << indentation << "radius: " << m_radius << std::endl;
		return true;
	}
}