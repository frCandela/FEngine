#pragma once

#include "network/fanNetworkPrecompiled.hpp"

namespace fan
{
	enum PacketType
	{
		PING = 0
		, LOGIN
		, LOGOUT
		, ACK_LOGIN
		, PLAYER_INPUT
		, START_GAME
		, COUNT
	};

	//================================================================================================================================
	// @wip
	//================================================================================================================================
	template< PacketType _Type >
	struct IPacket
	{
		virtual void LoadFrom( sf::Packet& /*_packet*/ ) {};

		sf::Packet ToPacket()
		{
			sf::Packet packet;
			ExportTo( packet );
			return packet;
		}

	protected:
		virtual void ExportTo( sf::Packet& _packet ) const
		{
			_packet << sf::Uint16( s_type );
		}

		static const PacketType s_type = _Type;
	};


	//================================================================================================================================
	// Empty packets
	//================================================================================================================================
	struct PacketAckLogin : IPacket<PacketType::ACK_LOGIN> {};
	struct PacketAckLogout : IPacket<PacketType::LOGOUT> {};
	struct PacketStartGame : IPacket<PacketType::START_GAME> {};
}