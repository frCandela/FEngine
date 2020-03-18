#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "game/fanGameSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	struct SceneNode;
	class Actor;
	class EcsManager;
	class SceneInstantiate;
	class Prefab;

	//================================================================================================================================
	//================================================================================================================================
	class Scene
	{
	public:
		Signal< Scene& >		onSceneLoad;
		Signal< Scene& >		onSceneStop;
		Signal<>				onSceneClear;
		Signal< SceneNode* >	onDeleteSceneNode;
		Signal< SceneNode& >	onSetMainCamera;

		uint32_t	nextUniqueID = 1;

		enum State { STOPPED, PLAYING, PAUSED };

		Scene( const std::string _name, void ( *_initializeTypesEcsWorld )( EcsWorld& ) );
		~Scene();


		SceneNode&  InstanciateSceneNode( const std::string _name, SceneNode* const _parentNode, const bool _generateID = true );
		SceneNode*  InstanciatePrefab( const Prefab& _prefab, SceneNode* const _parent );
		void		DeleteSceneNode( SceneNode& _node );

		void Update( const float _delta );

		void Play();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		void New();
		void Save() const;
		bool LoadFrom( const std::string _path );
		void SetPath( const std::string _path ) { m_path = _path; }

		SceneNode&  GetRootNode() { return *m_rootNode; };

		static uint32_t	R_FindMaximumId( SceneNode& _node );
		static void		R_SaveToJson( const SceneNode& _node, Json& _json );
		static void		R_LoadFromJson( const Json& _json, SceneNode& _node, const uint32_t _idOffset );
		static void		RemapSceneNodesIndices( Json& _json );

		inline std::string		 GetName() const { return m_name; }
		bool					 IsServer() const { return m_isServer; }
		bool					 HasPath() const { return m_path.empty() == false; }
		inline std::string		 GetPath() const { return m_path; }
		inline SceneInstantiate& GetInstanciator() const { return *m_instantiate;  }
		inline EcsManager&		 GetEcsManager() const { return * ((EcsManager*)0); }//@hack
		inline EcsWorld&		 GetWorld() { return m_world; }
		State					 GetState() const { return m_state; };
		SceneNode&				 GetMainCamera() { return *m_mainCamera; }
		void					 SetMainCamera( SceneNode& _nodeCamera );
		void					 SetServer( const bool _isServer ) { m_isServer = _isServer; }

	private:
		// Data
		std::string	m_name;
		std::string	m_path;		
		bool		m_isServer = false;
		State		m_state = State::STOPPED; // State
		SceneNode * m_rootNode = nullptr;
		SceneNode*	m_mainCamera = nullptr;
		std::vector < SceneNode* > m_sceneNodesToDelete;

		SceneInstantiate* m_instantiate = nullptr;
		EcsWorld          m_world = nullptr;

		void EndFrame();
		void Clear();
		void DeleteNodesImmediate( const std::vector<SceneNode*>& _nodes );
	};
}
