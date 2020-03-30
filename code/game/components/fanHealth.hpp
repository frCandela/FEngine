#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Health : public Component
	{
		DECLARE_COMPONENT( Health )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		Signal<> onFallToZero;
		bool  invincible = false;
		float currentHealth = 0.f;
		float maxHealth = 100.f;
	};

	//================================================================================================================================

//================================================================================================================================
// 	bool Health::TryRemoveHealth( const float _healthConsumed )
// 	{
// 		assert( _healthConsumed >= 0.f );
// 
// 		if ( m_invincible ) { return true; }
// 
// 		if ( m_currentHealth >= _healthConsumed )
// 		{
// 			m_currentHealth -= _healthConsumed;
// 			if ( m_currentHealth <= 0.f ) { onFallToZero.Emmit(); }
// 			return true;
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void Health::AddHealth( const float _healthAdded )
// 	{
// 		assert( _healthAdded >= 0.f );
// 		m_currentHealth = std::min( m_currentHealth + _healthAdded, m_maxHealth );
// 	}
}