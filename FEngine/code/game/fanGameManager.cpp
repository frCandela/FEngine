#include "fanGlobalIncludes.h"
#include "game/fanGameManager.h"

#include "core/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanScene.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
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
		m_camera = GetGameobject()->GetComponent<Camera>();
		if (m_camera == nullptr) {
			Debug::Warning("Game manager has no camera attached");
			SetEnabled(false);
		}

		m_spaceShip = GetGameobject()->GetScene()->FindComponentOfType<SpaceShip>();
		if (m_spaceShip == nullptr) {
			Debug::Warning("GameManager::Start : No spaceShip found");
			SetEnabled(false);
		}
		else {
			m_spaceShip->SetEnabled(false);
		}

		m_editorCameraController = GetGameobject()->GetScene()->FindComponentOfType<FPSCamera>();
		if (m_editorCameraController == nullptr) {
			Debug::Warning("GameManager::Start : No editor CameraController found");
			SetEnabled(false);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::SwitchCameras() {
		Debug::Log("switching camera");
		Camera * currentCamera = & Debug::Get().MainCamera();

		if ( currentCamera == & Debug::Get().EditorCamera() ) {
			Debug::Get().SetMainCamera(m_camera);
			m_spaceShip->SetEnabled(true);
			m_editorCameraController->SetEnabled(false);
		}
		else {
			Debug::Get().SetMainCamera( &Debug::Get().EditorCamera() );
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
	bool GameManager::Load( Json & _json ) {
		Actor::Load(_json);
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Save( Json & _json ) const {		
		Actor::Save( _json );		
		return true;
	}
}