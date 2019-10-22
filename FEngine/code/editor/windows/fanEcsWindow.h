#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan {
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================
	class EcsWindow : public EditorWindow {
	public:
		EcsWindow( EcsManager * _ecsManager );
	protected:
		void OnGui() override;

	private:
		EcsManager * m_ecsManager = nullptr;
	};

}