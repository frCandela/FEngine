#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan {

	class Scene;

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