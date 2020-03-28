#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "scene/fanPrefab.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsWorld;

// 	//================================================================================================================================
// 	// holds a pointer to a gameobject of a specific  scene
// 	// can be initialized with a scene and a gameobject ID to be resolved later ( loading, copy/paste )
// 	//================================================================================================================================
// 	class GameobjectPtr : public ResourcePtr<Gameobject>
// 	{
// 	public:
// 		GameobjectPtr(  Gameobject* _gameobject = nullptr ) : ResourcePtr<Gameobject>( _gameobject ) {}
// 		~GameobjectPtr();
// 
// 		void Init( Scene& _scene, uint64_t _gameobjectId );
// 
// 		uint64_t GetId() const	 { return m_gameobjectId; }
// 
// 		GameobjectPtr& operator=( Gameobject* _resource );
// 
// 	private:
// 		uint64_t m_gameobjectId = 0;
// 		Scene * m_scene = nullptr;
// 	};	  

	//================================================================================================================================
	// holds a pointer to a component of a specific  scene
	// can be initialized with a scene, scene node ID and a component ID to be resolved later ( loading, copy/paste )
	//================================================================================================================================
	struct ComponentPtrBase
	{
		ComponentPtrBase( const uint32_t _staticID ) : staticID(_staticID) {}
		void Init( EcsWorld& _world );
		void Create( uint32_t _sceneNodeID );
		void Create( SceneNode& _sceneNode, Component& _component );
		void Clear();

		const uint32_t		  staticID;				// static id of the component
		const ComponentIndex  dynamicID = 255;		// dynamic id of the component
		EcsWorld* const		  world	 = nullptr;		// world containing the target component
		uint32_t			  sceneNodeID = 0;		// unique index of the associated scene node
		Component*			  component = nullptr;  // the component
	}; static constexpr size_t sizeof_componentPtrBase = sizeof( ComponentPtrBase );

	//================================================================================================================================
	// Same functionality than ComponentPtrBase but strongly typed
	//================================================================================================================================
	template< typename _componentType>
	class ComponentPtr : public ComponentPtrBase
	{
	public:
		ComponentPtr() : ComponentPtrBase( _componentType::s_typeInfo ) {}
		// 		ComponentPtr( Component* _component = nullptr ) : ComponentPtrBase( _component ) {}
		// 		_ComponentType* operator->() const { return static_cast<_ComponentType*>(GetResource()); }
		// 		_ComponentType* operator*() const { return static_cast<_ComponentType*>(GetResource()); }
	};

	//================================================================================================================================
	// holds a pointer to a prefab of the prefab resource manager
	// can be initialized with a prefab path to be resolved later
	//================================================================================================================================
	class PrefabPtr : public ResourcePtr<Prefab>
	{
	public:
		PrefabPtr( Prefab* _prefab = nullptr ) : ResourcePtr<Prefab>( _prefab ) {}

		void Init( const std::string _path ) { m_path = _path; }
		const std::string& GetPath() const { return m_path; }

		ResourcePtr& operator=( Prefab* _resource ) { SetResource( _resource ); return *this; }
	private:
		std::string m_path;
	};
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
//	bool FanGameobject( const char* _label, fan::GameobjectPtr& _ptr );
	bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr );
 	bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr );
	template< typename _componentType >	bool FanComponent( const char* _label, fan::ComponentPtr<_componentType>& _ptr )
	{
		static_assert( ( std::is_base_of<fan::Component, _componentType>::value ) );
		return FanComponentBase( _label, _ptr );
	}
}
