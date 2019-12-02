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
		void SetScene( Scene * _scene ) { m_scene = _scene ;}

		void Update( const float _delta );
	protected:
		void OnGui() override;

	private:
		Scene * m_scene = nullptr;		

	};

}