#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class Gameobject;
	class GameWindow;
	class Scene;

	//================================================================================================================================
	//================================================================================================================================	
	class EditorSelection
	{
	public:
		Signal<Gameobject*> onGameobjectSelected;

		EditorSelection( Scene*& _currentScene );
		void ConnectCallbacks( Scene& _clientScene, Scene& _serverScene );

		void SetSelectedGameobject( Gameobject* _selectedGameobject );
		void Deselect();
		void DeleteSelection();
		void Update( const bool _gameWindowHovered );

		inline Gameobject* GetSelectedGameobject() const { return m_selectedGameobject; }
		inline Scene& GetSelectedScene()const { return *m_currentScene; }


	private:
		Gameobject* m_selectedGameobject;
		Scene*& m_currentScene;

		void OnGameobjectDeleted( Gameobject* _gameobject );
	};
}