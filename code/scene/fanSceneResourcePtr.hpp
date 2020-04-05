#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "scene/fanPrefab.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsWorld;	  

	//================================================================================================================================
	// holds a pointer to a component of a specific  scene
	// must be initialized with an EcsWorld
	// can be initialized with scene node/component directly or with a sceneNode ID  to be resolved later ( loading, copy/paste )
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
		_componentType* operator->() const { return static_cast<_componentType*>( component ); }
		_componentType& operator*() const { return *static_cast<_componentType*>( component ); }
		bool operator!=( const _componentType* _other ) const{ return _other != component;	}
		bool operator==( const _componentType* _other ) const{ return _other == component;	}
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
	bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr );
 	bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr );
	template< typename _componentType >	bool FanComponent( const char* _label, fan::ComponentPtr<_componentType>& _ptr )
	{
		static_assert( ( std::is_base_of<fan::Component, _componentType>::value ) );
		return FanComponentBase( _label, _ptr );
	}
}
