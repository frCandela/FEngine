#include "fanGlobalIncludes.h"
#include "game/fanGameManager.h"

#include "core/time/fanTime.h"
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
		m_camera = m_gameobject->GetComponent<Camera>();
		if (m_camera == nullptr) {
			Debug::Warning("Game manager has no camera attached");
			SetEnabled(false);
		}

		m_spaceShip = m_gameobject->GetScene()->FindComponentOfType<SpaceShip>();
		if (m_spaceShip == nullptr) {
			Debug::Warning("GameManager::Start : No spaceShip found");
			SetEnabled(false);
		}
		else {
			m_spaceShip->SetEnabled(false);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnAttach() {
		Actor::OnAttach();
		m_gameobject->GetScene()->onScenePlay.Connect( &GameManager::OnScenePlay, this );
		m_gameobject->GetScene()->onScenePause.Connect( &GameManager::OnScenePause, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnDetach() {
		Actor::OnDetach();
		m_gameobject->GetScene()->onScenePlay.Disconnect( &GameManager::OnScenePlay, this );
		m_gameobject->GetScene()->onScenePause.Disconnect( &GameManager::OnScenePause, this );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnScenePlay() {
		m_gameobject->GetScene()->SetMainCamera( m_camera );
		m_spaceShip->SetEnabled( true );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnScenePause() {
		m_spaceShip->SetEnabled( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::Update(const float /*_delta*/) {

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