#pragma once

#include "ecs/fanSignal.hpp"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	class GameViewWindow;
	struct Scene;
	struct SceneNode;

	//========================================================================================================
	// Manages the editor scene node selection
	//========================================================================================================
	struct EditorSelection : EcsSingleton
	{
		ECS_SINGLETON( EditorSelection );

		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		void ConnectCallbacks( Scene& _scene );
		void SetSelectedSceneNode( SceneNode* _node );
		void Deselect();
		void DeleteSelection();
		void Update( const bool _gameWindowHovered );

		SceneNode* GetSelectedSceneNode() const;
		Scene&	   GetSelectedScene() const { return *m_currentScene; }

		EcsHandle			m_selectedNodeHandle = 0;
		Scene*				m_currentScene = nullptr;

		void OnSceneNodeDeleted( SceneNode* _node );
		void OnToogleTransformLock();
	};
}