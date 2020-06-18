#pragma once

#include "ecs/fanEcsComponent.hpp"

#include <deque>
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// Save states of the entity for rolling back the simulation
	//================================================================================================================================	
	struct ClientRollback : public EcsComponent
	{
		ECS_COMPONENT( ClientRollback )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		//================================================================
		// holds the rollback state data for a specific frame
		//================================================================
		struct RollbackState
		{
			FrameIndex frameIndex;
			sf::Packet data;
		};

		FrameIndex GetNewestFrameIndex() const { return previousStates.empty() ? 0 : previousStates.back().frameIndex;  }
		FrameIndex GetOldestFrameIndex() const { return previousStates.empty() ? 0 : previousStates.front().frameIndex; }
		const RollbackState* GetState( const FrameIndex _frameIndex ) const;


		std::deque<RollbackState> previousStates;
	};
}