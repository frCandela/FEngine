#pragma  once

#include <queue>
#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server] All game info for a remote player
	//==============================================================================================================================================================
	struct HostGameData : public EcsComponent
	{
		ECS_COMPONENT( HostGameData )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		NetID									spaceshipID;
		EcsHandle								spaceshipHandle;
		std::queue< PacketInput::InputData >	inputs;
		PacketPlayerGameState					nextPlayerState;
		FrameIndex								nextPlayerStateFrame;	// the index of the next frame on which we will save the player game state

		void ProcessPacket( PacketInput& _packet );
	};
	static constexpr size_t sizeof_hostGameData = sizeof( HostGameData );
}