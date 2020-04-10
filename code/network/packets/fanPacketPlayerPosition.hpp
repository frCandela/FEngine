#pragma once
/*
#include "network/packets/fanIPacket.hpp"

namespace fan
{
	//================================================================================================================================
	// @wip
	//================================================================================================================================
	struct PacketPing : IPacket<PacketType::PING>
	{

		PacketPing( const float	_time = -1.f )
			: m_time( _time )
		{}

		void LoadFrom( sf::Packet& _packet ) override
		{
			_packet >> m_time;
		}

		inline float	GetTime() { return m_time; }

	protected:
		void ExportTo( sf::Packet& _packet ) const override
		{
			IPacket::ExportTo( _packet );
			_packet << m_time;
		}

		float		m_time;
	};
}*/