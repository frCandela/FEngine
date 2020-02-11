#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	class Component;
	class Actor;

	struct IDPtrData
	{
		IDPtrData() : gameobjectID( 0 ), componentID( 0 ) {}
		IDPtrData( uint64_t _gameobjectID, uint32_t _componentID ) : gameobjectID( _gameobjectID ), componentID( _componentID ) {}
		uint64_t gameobjectID;
		uint32_t componentID;
	};

	//================================================================================================================================
	// Component resource pointer storing a gameobject::uniqueid and component::typeid
	//================================================================================================================================
	using ComponentIDPtr = ResourcePtr< Component >;

	//================================================================================================================================
	// Like a ComponentIDPtr but strongly typed
	//================================================================================================================================
	template< typename _ComponentType >
	class ComponentPtr : public ComponentIDPtr
	{
	public:
		ComponentPtr( Component* _component = nullptr ) : ComponentIDPtr( static_cast< Resource< Component>* >( _component ) ) {}

		_ComponentType* operator->() const { return static_cast< _ComponentType* >( GetResource() ); }
		_ComponentType* operator*() const { return static_cast< _ComponentType* >( GetResource() ); }
	};
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	template< typename _ComponentType >
	bool FanComponent( const char* _label, fan::ComponentPtr<_ComponentType>* _ptr )
	{
		static_assert( ( std::is_base_of<fan::Component, _ComponentType>::value ) );

		return FanComponent( _label, _ComponentType::s_typeID, _ptr );
	}

	bool FanComponent( const char* _label, const uint32_t _typeID, fan::ComponentIDPtr* _ptr );
}
