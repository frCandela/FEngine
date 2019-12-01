#pragma once

#include "network/packets/fanIPacket.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct PacketPing : IPacket<PacketType::PING>
	{

		PacketPing( const float	_time = -1.f, const sf::Uint32 _frameIndex = 0 ) 
			: m_time( _time )
			, m_frameIndex( _frameIndex )
		{}

		void LoadFrom( sf::Packet & _packet ) override
		{
			_packet >> m_time >> m_frameIndex;
		}

		inline float	GetTime() { return m_time; }
		inline uint32_t GetFrame(){ return m_frameIndex; }

	protected:
		void ExportTo( sf::Packet & _packet ) const override
		{
			IPacket::ExportTo( _packet );
			_packet << m_time << m_frameIndex;
		}

		float		m_time;
		sf::Uint32	m_frameIndex;
	};
}