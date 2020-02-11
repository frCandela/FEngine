#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class Gameobject;
	class Component;

	//================================================================================================================================
	//================================================================================================================================
	class InspectorWindow : public EditorWindow
	{
	public:
		InspectorWindow();


		// Callbacks
		void OnGameobjectSelected( Gameobject* _gameobject ) { m_gameobjectSelected = _gameobject; }

	protected:
		void OnGui() override;

	private:
		Gameobject* m_gameobjectSelected;

		void NewComponentPopup();
		void R_NewComponentPopup( std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector< const Component*>& _components, const std::vector<std::filesystem::path>& _componentsPath );
		void NewComponentItem( const Component* _component );
	};
}