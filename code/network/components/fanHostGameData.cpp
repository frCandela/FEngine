#include "network/components/fanHostGameData.hpp"

namespace fan
{
	REGISTER_COMPONENT( HostGameData, "host game data" );

	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.onGui = &HostGameData::OnGui;
		_info.init =  &HostGameData::Init;
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::Init( EcsWorld& _world, Component& _component )
	{
		HostGameData& hostGameData = static_cast<HostGameData&>( _component );
		hostGameData.spaceshipID = 0;
		hostGameData.spaceshipHandle = 0;
		hostGameData.inputs = std::queue<PacketInput>();
		hostGameData.nextPlayerState = PacketPlayerGameState();
	}

	//================================================================================================================================
	//================================================================================================================================
	void HostGameData::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		HostGameData& hostGameData = static_cast<HostGameData&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "spaceshipID :      %u", hostGameData.spaceshipID );
			ImGui::Text( "spaceship handle : %u", hostGameData.spaceshipHandle );
			ImGui::Text( "inputs size :      %u", hostGameData.inputs.size() );
		} ImGui::PopItemWidth();
	}
}