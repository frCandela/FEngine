#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"
#include "scene/fanSceneRessourcePtr.hpp"

namespace fan
{
	class Component;

	struct IDPtrData
	{
		IDPtrData() : gameobjectID( 0 ), componentID( 0 ) {}
		IDPtrData( uint64_t _gameobjectID, uint32_t _componentID ) : gameobjectID( _gameobjectID ), componentID( _componentID ) {}
		uint64_t gameobjectID;
		uint32_t componentID;
	};

	//================================================================================================================================
	// Component ressource pointer storing a gameobject::uniqueid and component::typeid
	//================================================================================================================================
	using ComponentIDPtr = RessourcePtr< Component, IDPtrData >;

	//================================================================================================================================
	// Like a ComponentIDPtr but strongly typed
	//================================================================================================================================
	template< typename _ComponentType >
	class ComponentPtr : public ComponentIDPtr
	{
	public:
		ComponentPtr( _ComponentType * _ressourceType, IDPtrData _ressourceID ) : ComponentIDPtr( _ressourceType, _ressourceID ) {}
		ComponentPtr( _ComponentType * _component ) : ComponentIDPtr( _component, IDPtrData(_component->GetGameobject()->GetUniqueID(), _component->GetType())  ) {}
		ComponentPtr() : ComponentIDPtr() {}

		_ComponentType* operator->() const { return static_cast<_ComponentType*>( GetRessource() ); }
		_ComponentType* operator*() const { return static_cast<_ComponentType*>( GetRessource() ); }
	};
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	template< typename _ComponentType >
	bool FanComponent( const char * _label, fan::ComponentPtr<_ComponentType> * _ptr )
	{
		static_assert( ( std::is_base_of<fan::Component, _ComponentType>::value ) );

		return FanComponent( _label, _ComponentType::s_typeID, _ptr );
	}

	bool FanComponent( const char * _label, const uint32_t _typeID, fan::ComponentIDPtr * _ptr );
}
