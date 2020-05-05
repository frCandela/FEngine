#pragma once

#include "ecs/fanComponent.hpp"
#include "bullet/LinearMath/btVector3.h"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// Stores the input of the player
	// This input can be direct in case of a local player or replicated ( server or remote players )
	//================================================================================================================================
	struct PlayerInput : public Component
	{
		DECLARE_COMPONENT( PlayerInput )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		btVector3	orientation; // orientation of the ship
		float		left;		 // left/right key pressed ( strafing )
		float		forward;	 // forward or backward
		float		boost;		 // shift to go faster
		float		fire;		 // firing in front of the ship
	};
}