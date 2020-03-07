#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/fanSignal.hpp"
#include "game/fanGameSerializable.hpp"

namespace fan
{
	struct SceneNode;
	class Gameobject;
	class Component;
	class Actor;
	class EcsManager;
	class PhysicsManager;
	class EntityWorld;
	class SceneInstantiate;
	class Prefab;

	class MeshRenderer;
	class PointLight;
	class DirectionalLight;

	class GameobjectPtr;

	//================================================================================================================================
	//================================================================================================================================
	class Scene
	{
	public:
		Signal< Scene* >		onSceneLoad;
		Signal< Scene* >		onSceneStop;
		Signal<>				onSceneClear;
		Signal< SceneNode* >	onDeleteSceneNode;
		Signal< Camera* >		onSetMainCamera;

		Signal< MeshRenderer* >		onRegisterMeshRenderer;
		Signal< MeshRenderer* >		onUnRegisterMeshRenderer;
		Signal< PointLight* >		onPointLightAttach;
		Signal< PointLight* >		onPointLightDetach;
		Signal< DirectionalLight* > onDirectionalLightAttach;
		Signal< DirectionalLight* > onDirectionalLightDetach;

		enum State { STOPPED, PLAYING, PAUSED };

		Scene( const std::string _name );
		~Scene();


		SceneNode&  CreateSceneNode( const std::string _name, SceneNode* const _parentNode, const uint64_t _uniqueId = 0 );
		void		DeleteSceneNode( SceneNode& _node );
		Gameobject* CreateGameobject( const std::string _name, Gameobject* const _parent, const uint64_t _uniqueId = 0 );
		Gameobject* CreateGameobject( const Prefab& _prefab, Gameobject* const _parent );
		void		DeleteGameobject( Gameobject* _gameobject );
		void		DeleteComponent( Component* _component ) { m_componentsToDelete.push_back( _component ); } // Delete at the end of the frame
		std::vector < Gameobject* >	BuildEntitiesList() const;

		template<typename _componentType> _componentType*				FindComponentOfType() const;
		template<typename _componentType> std::vector<_componentType*>  FindComponentsOfType() const;


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

		Gameobject* GetRoot() { return m_root; }
		SceneNode& GetRootNode() { return *m_rootNode; };

		inline std::string		 GetName() const { return m_name; }
		bool					 IsServer() const { return m_isServer; }
		bool					 HasPath() const { return m_path.empty() == false; }
		inline std::string		 GetPath() const { return m_path; }
		inline SceneInstantiate& GetInstanciator() const { return *m_instantiate;  }
		inline EcsManager&		 GetEcsManager() const { return * ((EcsManager*)0); }//@hack
		inline EntityWorld&		 GetEntityWorld() const { return *m_world; }
		inline PhysicsManager&	 GetPhysicsManager() const { return *m_physicsManager; }
		State					 GetState() const { return m_state; };
		Camera&					 GetMainCamera() { return *m_mainCamera; }
		void					 SetMainCamera( Camera* _camera );
		void					 SetServer( const bool _isServer ) { m_isServer = _isServer; }		
		Gameobject*				 FindGameobject( const uint64_t _id );

		uint64_t GetNextUniqueID() const { return m_nextUniqueID; }

		const std::vector < DirectionalLight* >& GetDirectionalLights() { return m_directionalLights; }
		const std::vector < PointLight* >&		 GetPointLights() { return m_pointLights; }
		const std::vector < MeshRenderer* >&	 GetMeshRenderers() { return m_meshRenderers; }

		void Enable( Actor* _actor );
		void Disable( Actor* _actor );

		// Scene callbacks
		void RegisterActor( Actor* _actor );
		void UnregisterActor( Actor* _actor );
		void RegisterDirectionalLight( DirectionalLight* _pointLight );
		void UnRegisterDirectionalLight( DirectionalLight* _pointLight );
		void RegisterPointLight( PointLight* _pointLight );
		void UnRegisterPointLight( PointLight* _pointLight );
		void RegisterMeshRenderer( MeshRenderer* _meshRenderer );
		void UnRegisterMeshRenderer( MeshRenderer* _meshRenderer );

	private:
		// Data
		std::string	m_name;
		std::string	m_path;
		uint64_t	m_nextUniqueID = 1;
		bool		m_isServer = false;

		SceneInstantiate* m_instantiate = nullptr;
		PhysicsManager* m_physicsManager = nullptr;
		EntityWorld* m_world = nullptr;

		// References
		SceneNode * m_rootNode = nullptr;
		Gameobject* m_root;
		Camera* m_mainCamera = nullptr;

		// State
		State m_state = State::STOPPED;

		// Gameobjects
		std::vector < Gameobject* >			m_entitiesToDelete;
		std::vector < SceneNode* >			m_sceneNodesToDelete;
		std::vector < Component* >			m_componentsToDelete;
		std::vector< Actor* >				m_actors;
		std::vector< Actor* >				m_startingActors;
		std::vector< Actor* >				m_activeActors;
		std::vector< Actor* >				m_pausedActors;
		std::map< uint64_t, Gameobject* >	m_gameobjects;

		// registered elements
		std::vector < DirectionalLight* >	m_directionalLights;
		std::vector < PointLight* >			m_pointLights;
		std::vector < MeshRenderer* >		m_meshRenderers;

		void BeginFrame();
		void UpdateActors( const float _delta );
		void LateUpdateActors( const float _delta );
		void EndFrame();

		uint64_t NextUniqueID() { assert( FindGameobject( m_nextUniqueID ) == nullptr );  return m_nextUniqueID++; }

		bool R_Load( const Json&	  _json, SceneNode& _node );
		bool R_Save( const SceneNode& _node, Json& _json ) const;
		void Clear();
		void DeleteNodesImmediate( const std::vector<SceneNode*>& _nodes );

		// @todo, place all static scene utility (below) in a separate file
		template<typename _componentType>
		void		R_FindComponentsOfType( const Gameobject* _gameobject, std::vector<_componentType*>& _components ) const;
		void		R_BuildEntitiesList( Gameobject* _gameobject, std::vector<Gameobject*>& _entitiesList ) const;
		Component*	R_FindComponentOfType( Gameobject* _gameobject, const uint32_t _typeID ) const;
		uint64_t	R_FindMaximumId( Gameobject& _gameobject );
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename _componentType>
	_componentType* Scene::FindComponentOfType() const
	{
		static_assert( ( std::is_base_of<Component, _componentType>::value ) );
		return  static_cast< _componentType* > ( R_FindComponentOfType( m_root, _componentType::s_typeID ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename _componentType>
	std::vector<_componentType*> Scene::FindComponentsOfType() const
	{
		static_assert( ( std::is_base_of<Component, _componentType>::value ) );
		std::vector<_componentType*> vector;
		R_FindComponentsOfType<_componentType>( m_root, vector );
		return  vector;
	}


	//================================================================================================================================
	//================================================================================================================================
	template<typename _componentType>
	void Scene::R_FindComponentsOfType( const Gameobject* _gameobject, std::vector<_componentType*>& _components ) const
	{

		// Search in components
		const std::vector<Component*>& components = _gameobject->GetComponents();
		for ( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
		{
			Component* component = components[ componentIndex ];
			if ( component->IsType< _componentType >() )
			{
				_components.push_back( static_cast< _componentType* >( component ) );
			}
		}

		// Recursive call to child entities
		const std::vector<Gameobject*>& childEntities = _gameobject->GetChilds();
		for ( int childIndex = 0; childIndex < childEntities.size(); childIndex++ )
		{
			R_FindComponentsOfType<_componentType>( childEntities[ childIndex ], _components );
		}
	}
}
