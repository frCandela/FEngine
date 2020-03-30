#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//================================================================================================================================
	// a battery allows fast moving of ships & firing weapons
	//================================================================================================================================
	struct Battery : public Component
	{
		DECLARE_COMPONENT( Battery )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float currentEnergy = 0.f;
		float maxEnergy = 100.f;
	};
	static constexpr size_t sizeof_battery = sizeof( Battery );
}


// ================================================================================================================================
// Try to remove energy 
// If there is not enough energy available, do nothing and return false
// ================================================================================================================================
// bool Battery::TryRemoveEnergy( const float _energyConsumed )
// {
// 	assert( _energyConsumed >= 0.f );
// 
// 	if( m_currentEnergy >= _energyConsumed )
// 	{
// 		m_currentEnergy -= _energyConsumed;
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }
// 
// ================================================================================================================================
// ================================================================================================================================
// void Battery::AddEnergy( const float _energyAdded )
// {
// 	assert( _energyAdded >= 0.f );
// 	m_currentEnergy = std::min( m_currentEnergy + _energyAdded, m_maxEnergy );
// }