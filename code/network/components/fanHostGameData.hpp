#pragma  once

#include <queue>
#include "core/ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//========================================================================================================
	// [Server] All game info for a remote player
	//========================================================================================================
	struct HostGameData : public EcsComponent
	{
		ECS_COMPONENT( HostGameData )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

		NetID                                mSpaceshipID;
		EcsHandle                            mSpaceshipHandle;
		std::queue< PacketInput::InputData > mInputs;
		PacketPlayerGameState                mNextPlayerState;
		FrameIndex                           mNextPlayerStateFrame; // save  player game state on this frame

		void ProcessPacket( PacketInput& _packet );
	};
}