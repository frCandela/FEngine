#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	// holds a pointer to a component of a specific  scene
	// can be initialized with a scene, gameobject ID and a component ID to be resolved later ( loading, copy/paste )
	//================================================================================================================================
// 	class ComponentPtrBase : public ResourcePtr<Component>
// 	{
// 	public:
// 		ComponentPtrBase( Component* _component = nullptr ) : ResourcePtr<Component>( _component ) {}
// 		~ComponentPtrBase() override;
// 
// 		void Init( Scene& _scene, uint64_t _gameobjectId, uint32_t _componentId );
// 
// 		uint64_t GetGameobjectId() const { return m_gameobjectId; }
// 		uint32_t GetComponentId() const { return m_componentId; }
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
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
// 	template< typename _ComponentType >
// 	bool FanComponent( const char* _label, fan::ComponentPtr<_ComponentType>& _ptr )
// 	{
// 		static_assert((std::is_base_of<fan::Component, _ComponentType>::value));
// 
// 		return FanComponent( _label, _ComponentType::s_typeID, _ptr );
// 	}
// 
// 	bool FanComponent( const char* _label, const uint32_t _typeID, fan::ComponentPtrBase& _ptr );
}
