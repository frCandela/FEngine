#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
	struct Scene;

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
// 	class ComponentPtrBase : public ResourcePtr<Component>
// 	{
// 	public:
// 		ComponentPtrBase( Component* _component = nullptr ) : ResourcePtr<Component>( _component ) {}
// 		~ComponentPtrBase() override;
// 
// 		void Init( Scene& _scene, uint64_t _gameobjectId, uint32_t _componentId );
// 
// 		uint64_t GetSceneNodeID() const { return m_gameobjectId; }
// 		uint32_t GetComponentID() const { return m_componentId; }
// 
// 		ComponentPtrBase& operator=( Component* _component );
// 
// 	private:
// 		uint64_t m_gameobjectId = 0;
// 		uint32_t m_componentId = 0;
// 		Scene* m_scene = nullptr;
// 	};

	//================================================================================================================================
	// Same functionality than ComponentPtrBase but strongly typed
	//================================================================================================================================
// 	template< typename _ComponentType>
// 	class ComponentPtr : public ComponentPtrBase {
// 	public:
// 		ComponentPtr( Component* _component = nullptr ) : ComponentPtrBase( _component ) {}
// 		_ComponentType* operator->() const { return static_cast<_ComponentType*>(GetResource()); }
// 		_ComponentType* operator*() const { return static_cast<_ComponentType*>(GetResource()); }
// 	};

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
	// 	template< typename _ComponentType >
// 	bool FanComponent( const char* _label, fan::ComponentPtr<_ComponentType>& _ptr )
// 	{
// 		static_assert((std::is_base_of<fan::Component, _ComponentType>::value));
// 
// 		return FanComponent( _label, _ComponentType::s_typeID, _ptr );
// 	}
// 
// 	bool FanComponent( const char* _label, const uint32_t _typeID, fan::ComponentPtrBase& _ptr );

	bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr );
}
