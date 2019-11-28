#include "fanGlobalIncludes.h"
#include "game/fanGameManager.h"

#include "core/time/fanTime.h"
#include "core/input/fanKeyboard.h"
#include "scene/fanScene.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"
#include "editor/components/fanFPSCamera.h"
#include "game/fanSpaceShip.h"	
#include "renderer/fanRenderer.h"


namespace fan {
	REGISTER_TYPE_INFO(GameManager, TypeInfo::Flags::EDITOR_COMPONENT, "game/managers/")

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::Start() 
	{
		if (*m_gameCamera == nullptr) {
			Debug::Warning("Game manager has no camera attached");
			SetEnabled(false);
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
		m_gameobject->GetScene()->SetMainCamera( *m_gameCamera );
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnScenePause() {
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::Update(const float /*_delta*/) {

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnGui() {
		Actor::OnGui();
		
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanComponent( "game camera", &m_gameCamera );
			ImGui::Text("Press tab to switch cameras");			
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Load( const Json & _json ) {
		Actor::Load(_json);
		LoadComponentPtr( _json, "game_camera", m_gameCamera );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Save( Json & _json ) const {		
		Actor::Save( _json );	
		SaveComponentPtr( _json, "game_camera", m_gameCamera );
		return true;
	}
}