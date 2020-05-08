#pragma  once

#include <queue>
#include "ecs/fanComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server] All game info for a remote player
	//==============================================================================================================================================================
	struct HostGameData : public Component
	{
		DECLARE_COMPONENT( HostGameData )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );

		NetID									spaceshipID;
		EntityHandle							spaceshipHandle;
		std::queue< PacketInput::InputData >	inputs;
		PacketPlayerGameState					nextPlayerState;

		void ProcessPacket( PacketInput& _packet );
	};
	static constexpr size_t sizeof_hostGameData = sizeof( HostGameData );
}