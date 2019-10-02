#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Entity;

	//================================================================================================================================
	//================================================================================================================================
	class InspectorWindow : public EditorWindow {
	public:
		InspectorWindow();


		// Callbacks
		void OnEntitySelected( Entity* _entity ) { m_entitySelected = _entity; }

	protected:
		void OnGui() override;

	private:
		Entity * m_entitySelected;

		void NewComponentPopup();
	};
}