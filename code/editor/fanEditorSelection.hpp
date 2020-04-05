#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	class GameViewWindow;
	struct Scene;
	struct SceneNode;

	//================================================================================================================================
	// Manages the editor scene node selection
	// @todo make this a singleton component in the editor ecs world
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