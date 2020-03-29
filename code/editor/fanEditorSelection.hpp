#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class GameWindow;
	struct Scene;
	struct SceneNode;

	//================================================================================================================================
	//================================================================================================================================	
	class EditorSelection
	{
	public:
		Signal<SceneNode*> onSceneNodeSelected;

		EditorSelection( Scene& _currentScene );
		void ConnectCallbacks( Scene& _scene );

		void SetSelectedSceneNode( SceneNode* _node );
		void Deselect();
		void DeleteSelection();
		void Update( const bool _gameWindowHovered );

		inline SceneNode* GetSelectedSceneNode() const { return m_selectedSceneNode; }
		inline Scene& GetSelectedScene() const { return *m_currentScene; }


	private:
		SceneNode* m_selectedSceneNode;
		Scene* m_currentScene;

		void OnSceneNodeDeleted( SceneNode* _node );
		void OnToogleTransformLock();
	};
}