#pragma once

#include "editor/windows/fanEditorWindow.h"
#include "network/fanClient.h"
#include "network/fanServer.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class NetworkWindow : public EditorWindow {
	public:

		NetworkWindow();

		void Update( const float _delta );

	protected:
		void OnGui() override;

	private:
		Client m_client1;
		Server m_server;
	};

}