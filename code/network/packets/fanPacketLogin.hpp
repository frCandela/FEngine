#pragma once

#include "network/packets/fanIPacket.hpp"

namespace fan
{
	//================================================================================================================================
	// @wip
	//================================================================================================================================
	struct PacketLogin : IPacket<PacketType::LOGIN>
	{
		PacketLogin( const std::string _name = "" )
			: m_name( _name )
		{}

		void LoadFrom( sf::Packet& _packet ) override
		{
			_packet >> m_name;
		}

		inline const std::string& GetName() const { return m_name; }

	private:
		void ExportTo( sf::Packet& _packet ) const override
		{
			IPacket::ExportTo( _packet );
			_packet << m_name;
		}

		std::string m_name;
	};
}