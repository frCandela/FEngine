#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Gameobject;

	//================================================================================================================================
	//================================================================================================================================
	class InspectorWindow : public EditorWindow {
	public:
		InspectorWindow();


		// Callbacks
		void OnGameobjectSelected( Gameobject* _gameobject ) { m_gameobjectSelected = _gameobject; }

	protected:
		void OnGui() override;

	private:
		Gameobject * m_gameobjectSelected;

		void NewComponentPopup();
	};
}