#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "bullet/LinearMath/btVector3.h"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	// Stores the input of the player
	// This input can be direct in case of a local player or replicated ( server or remote players )
	//========================================================================================================
	struct PlayerInput : public EcsComponent
	{
		ECS_COMPONENT( PlayerInput )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		static void RollbackSave( const EcsComponent& _component, sf::Packet& _packet );
		static void RollbackLoad( EcsComponent& _component, sf::Packet& _packet );

		btVector3 mOrientation; // orientation of the ship
		float     mLeft;		 // left/right key pressed ( strafing )
		float     mForward;	 // forward or backward
		float     mBoost;		 // shift to go faster
		float     mFire;		 // firing in front of the ship
	};
}