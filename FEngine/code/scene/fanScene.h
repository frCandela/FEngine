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
	class SceneInstantiate;

	class MeshRenderer;
	class PointLight;
	class DirectionalLight;

	template< typename _RessourceType, typename _IDType > class RessourcePtr;
	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t>;

	//================================================================================================================================
	//================================================================================================================================
	class Scene : public ISerializable
	{
	public:

		Signal<Scene*>			onSceneLoad;
		Signal<Scene*>			onSceneStop;
		Signal<>				onSceneClear;
		Signal< Gameobject *>	onDeleteGameobject;
		Signal<Camera*>			onSetMainCamera;

		Signal< MeshRenderer * > onRegisterMeshRenderer;
		Signal< MeshRenderer * > onUnRegisterMeshRenderer;
		Signal< PointLight * > onPointLightAttach;
		Signal< PointLight * > onPointLightDetach;
		Signal< DirectionalLight * > onDirectionalLightAttach;
		Signal< DirectionalLight * > onDirectionalLightDetach;

		enum State{ STOPPED, PLAYING, PAUSED };

		Scene( const std::string _name );
		~Scene();

		Gameobject *					CreateGameobject( const std::string _name, Gameobject * _parent = nullptr, const bool _generateID = true );	
		Gameobject *					CreateGameobject( const Prefab& _prefab,  Gameobject * _parent = nullptr, const bool _generateID = true );	
		void							DeleteGameobject(Gameobject* _gameobject);										
		std::vector < Gameobject * >	BuildEntitiesList() const;

		template<typename _componentType> _componentType *				 FindComponentOfType() const;
		template<typename _componentType> std::vector<_componentType *>  FindComponentsOfType() const;

		void BeginFrame();
		void Update(const float _delta);
		void LateUpdate( const float _delta );
		void EndFrame();
		void Play()	;
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

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
		State					GetState() const			{ return m_state; };
		Camera *				GetMainCamera()				{ return m_mainCamera; }
		void					SetMainCamera( Camera * _camera );
		uint64_t				GetUniqueID() { assert(FindGameobject(m_nextUniqueID)==nullptr);  return m_nextUniqueID++; }
		Gameobject *			FindGameobject( const uint64_t _id );

		void					InsertID( const uint64_t _id, Gameobject * _gameobject );
		void					EraseID( const uint64_t _id ) { m_gameobjects.erase( _id ); }

		const std::vector < DirectionalLight* >	& GetDirectionalLights(){ return m_directionalLights ;}
		const std::vector < PointLight* >		& GetPointLights()		{ return m_pointLights ;}
		const std::vector < MeshRenderer* >		& GetMeshRenderers()	{ return m_meshRenderers ;}

		void Enable(  Actor * _actor );
		void Disable(  Actor * _actor );

		void RegisterActor( Actor * _actor );
		void UnregisterActor( Actor * _actor );
		void RegisterDirectionalLight	( DirectionalLight * _pointLight );
		void UnRegisterDirectionalLight	( DirectionalLight * _pointLight );
		void RegisterPointLight			( PointLight *		 _pointLight );
		void UnRegisterPointLight		( PointLight *		 _pointLight );
		void RegisterMeshRenderer		( MeshRenderer *	 _meshRenderer );
		void UnRegisterMeshRenderer		( MeshRenderer *	 _meshRenderer );
	private:
		// Data
		std::string	m_name;
		std::string	m_path;
		uint64_t	m_nextUniqueID = 1;

		SceneInstantiate * m_instantiate = nullptr;
		EcsManager * 	   m_ecsManager = nullptr;
		PhysicsManager *   m_physicsManager = nullptr;

		// References
		Gameobject *			m_root;
		Camera *				m_mainCamera = nullptr;

		// State
		State m_state = State::STOPPED;

		// Gameobjects
		std::vector < Gameobject * >		m_entitiesToDelete;
		std::vector < GameobjectPtr * >		m_unresolvedGameobjectPointers;
		std::vector < ComponentIDPtr * >	m_unresolvedComponentPointers;
		std::vector< Actor * >				m_actors;
		std::vector< Actor * >				m_startingActors;
		std::vector< Actor * >				m_activeActors;
		std::vector< Actor * >				m_pausedActors;
		std::map< uint64_t, Gameobject * >	m_gameobjects;

		// registered elements
		std::vector < DirectionalLight* >	m_directionalLights;
		std::vector < PointLight* >			m_pointLights;
		std::vector < MeshRenderer* >		m_meshRenderers;

		void UpdateActors(const float _delta);

		void OnGameobjectPtrCreate( GameobjectPtr * _gameobjectPtr );
		void ResolveGameobjectPointers( );
		void OnResolveComponentIDPtr( ComponentIDPtr * _ptr );

		bool Load( const Json & _json ) override;
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
