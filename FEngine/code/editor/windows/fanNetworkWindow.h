#pragma once

#include "editor/windows/fanEditorWindow.h"
#include "network/fanClient.h"
#include "network/fanServer.h"

namespace fan {

	class Scene;
	class GameServer;

	//================================================================================================================================
	//================================================================================================================================
	class NetworkWindow : public EditorWindow {
	public:

		NetworkWindow( );

		void Update( const float _delta );

		void OnSceneLoad( Scene * _scene );
		void OnSceneClear();
	protected:
		void OnGui() override;

	private:
		Scene * m_scene = nullptr;
		GameServer * m_gameServer = nullptr;



	};

}