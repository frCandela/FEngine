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

		NetworkWindow( Scene * _clientScene, Scene * _serverScene);
	protected:
		void OnGui() override;

	private:
		Scene * m_clientScene = nullptr;
		Scene * m_serverScene = nullptr;
	};

}