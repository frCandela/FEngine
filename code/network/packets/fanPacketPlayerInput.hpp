#pragma once

#include "network/packets/fanIPacket.hpp"

namespace fan
{
	//================================================================
	// @wip
	//================================================================
	struct InputData
	{
		btVector3	direction;
		float		left;
		float		forward;
		float		boost;
		float		fire;
		bool		stop;
	};

	//================================================================================================================================
	// @wip
	//================================================================================================================================
	struct PacketPlayerInput : IPacket<PacketType::PLAYER_INPUT>
	{

		PacketPlayerInput( InputData _inputData )
			: m_inputData( _inputData )
		{}

		void LoadFrom( sf::Packet& _packet ) override
		{
			_packet >> m_inputData.direction[ 0 ];
			_packet >> m_inputData.direction[ 1 ];
			_packet >> m_inputData.direction[ 2 ];
			_packet >> m_inputData.left;
			_packet >> m_inputData.forward;
			_packet >> m_inputData.boost;
			_packet >> m_inputData.fire;
			_packet >> m_inputData.stop;
		}

		inline const InputData& GetInputData() const { return m_inputData; }

	protected:
		void ExportTo( sf::Packet& _packet ) const override
		{
			IPacket::ExportTo( _packet );
			_packet << m_inputData.direction[ 0 ];
			_packet << m_inputData.direction[ 1 ];
			_packet << m_inputData.direction[ 2 ];
			_packet << m_inputData.left;
			_packet << m_inputData.forward;
			_packet << m_inputData.boost;
			_packet << m_inputData.fire;
			_packet << m_inputData.stop;
		}

		InputData		m_inputData;
	};
}