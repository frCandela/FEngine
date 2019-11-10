#pragma once

#include "core/fanSignal.h"
#include "core/fanISerializable.h"

namespace fan
{
	class Gameobject;
	class Component;
	class Actor;
	class EcsManager;
	class PhysicsManager;
	class GameobjectPtr;

	//================================================================================================================================
	//================================================================================================================================
	class Scene : public ISerializable
	{
	public:

		Signal<Scene*>			onSceneLoad;
		Signal<>				onSceneClear;
		Signal< Gameobject *>	onDeleteGameobject;
		Signal<Camera*>			onSetMainCamera;
		Signal<>				onScenePlay;
		Signal<>				onScenePause;

		Scene( const std::string _name, EcsManager * _ecsManager, PhysicsManager * _physicsManager );
		~Scene();

		Gameobject *					CreateGameobject(const std::string _name, Gameobject * _parent = nullptr);	// Creates a game object and adds it to the scene hierarchy
		void							DeleteGameobject(Gameobject* _gameobject);										// Deletes a gameobject and removes it from the scene hierarchy at the end of the frame
		std::vector < Gameobject * >	BuildEntitiesList() const;

		template<typename _componentType> _componentType *				 FindComponentOfType() const;
		template<typename _componentType> std::vector<_componentType *>  FindComponentsOfType() const;

		void BeginFrame();
		void Update(const float _delta);
		void LateUpdate( const float _delta );
		void EndFrame();
		void Play()	;
		void Pause();
		void OnGui();

		void New();
		void Save() const;
		bool LoadFrom(const std::string _path);
		void SetPath( const std::string _path ) { m_path = _path; }

		Gameobject *			GetRoot()					{ return m_root; }
		inline std::string		GetName() const				{ return m_name; }
		bool					HasPath() const				{ return m_path.empty() == false; }
		inline std::string		GetPath() const				{ return m_path; }
		inline EcsManager *		GetEcsManager() const		{ return m_ecsManager; }
		inline PhysicsManager *	GetPhysicsManager() const	{ return m_physicsManager; }		
		bool					IsPaused() const			{ return m_isPaused; };
		Camera *				GetMainCamera()				{ return m_mainCamera; }
		void					SetMainCamera( Camera * _camera );
		uint64_t				GetUniqueID() { return m_nextUniqueID++; }
		Gameobject *			FindGameobject( const uint64_t _id );

		void					InsertID( const uint64_t _id, Gameobject * _gameobject );
		void					EraseID( const uint64_t _id ) { m_gameobjects.erase( _id ); }
	private:
		// Data
		std::string	m_name;
		std::string	m_path;
		uint64_t	m_nextUniqueID = 1;

		// References
		Gameobject *			m_root;
		EcsManager * const		m_ecsManager = nullptr;
		PhysicsManager * const  m_physicsManager = nullptr;
		Camera *				m_mainCamera = nullptr;

		// State
		bool m_isPaused = false;

		// Gameobjects
		std::vector < Gameobject * >		m_entitiesToDelete;
		std::vector < GameobjectPtr * >		m_unresolvedGameobjectPointers;
		std::set< Actor * >					m_startingActors;
		std::set< Actor * >					m_activeActors;
		std::map< uint64_t, Gameobject * >	m_gameobjects;

		void OnActorAttach(Actor * _actor);
		void OnActorDetach(Actor * _actor);
		void OnGameobjectPtrCreate( GameobjectPtr * _gameobjectPtr );
		void ResolveGameobjectPointers( );

		bool Load( Json & _json ) override;
		bool Save( Json& _json ) const override;
		void Clear();

		void		R_DeleteGameobject(Gameobject* _gameobject, std::set<Gameobject*>&	_deletedEntitiesSet);
		void		R_BuildEntitiesList(Gameobject* _gameobject, std::vector<Gameobject*>& _entitiesList) const;
		Component *	R_FindComponentOfType(Gameobject * _gameobject, const uint32_t _typeID) const;

		template<typename _componentType>
		void	R_FindComponentsOfType( const Gameobject * _gameobject, std::vector<_componentType *> & _components ) const;
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename _componentType>
	_componentType * Scene::FindComponentOfType() const {
		static_assert((std::is_base_of<Component, _componentType>::value));
		return  static_cast<_componentType*> (R_FindComponentOfType(m_root, _componentType::s_typeID));
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename _componentType>
	std::vector<_componentType *> Scene::FindComponentsOfType() const {
		static_assert( ( std::is_base_of<Component, _componentType>::value ) );
		std::vector<_componentType *> vector;
		R_FindComponentsOfType<_componentType>( m_root, vector );
		return  vector;
	}


	//================================================================================================================================
	//================================================================================================================================
	template<typename _componentType>
	void Scene::R_FindComponentsOfType( const Gameobject * _gameobject, std::vector<_componentType *> & _components ) const {

		// Search in components
		const std::vector<Component*> & components = _gameobject->GetComponents();
		for ( int componentIndex = 0; componentIndex < components.size(); componentIndex++ ) {
			Component* component = components[componentIndex];
			if ( component->IsType< _componentType >() ) {
				_components.push_back( static_cast<_componentType*>( component ) );
			}
		}

		// Recursive call to child entities
		const std::vector<Gameobject*> & childEntities = _gameobject->GetChilds();
		for ( int childIndex = 0; childIndex < childEntities.size(); childIndex++ ) {
			R_FindComponentsOfType<_componentType>( childEntities[childIndex], _components );
		}
	}
}
