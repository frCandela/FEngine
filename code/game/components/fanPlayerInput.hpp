#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "bullet/LinearMath/btVector3.h"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// Stores the input of the player
	// This input can be direct in case of a local player or replicated ( server or remote players )
	//================================================================================================================================
	struct PlayerInput : public EcsComponent
	{
		ECS_COMPONENT( PlayerInput )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		btVector3	orientation; // orientation of the ship
		float		left;		 // left/right key pressed ( strafing )
		float		forward;	 // forward or backward
		float		boost;		 // shift to go faster
		float		fire;		 // firing in front of the ship
	};
	static constexpr size_t sizeof_playerInput = sizeof( PlayerInput );
}