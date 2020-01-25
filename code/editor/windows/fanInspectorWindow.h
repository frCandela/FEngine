#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Gameobject;
	class Component;

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
		void R_NewComponentPopup( std::set< std::fs::path >& _componentsPathSet, std::set< std::fs::path >::iterator&  _current, const std::vector< const Component *>& _components, const std::vector<std::fs::path>& _componentsPath );
		void NewComponentItem( const Component* _component );	
	};
}