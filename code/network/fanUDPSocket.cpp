#include "network/fanUDPSocket.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	UdpSocket::UdpSocket()
	{
		mSocket.setBlocking( false );
	}


    //========================================================================================================
    //========================================================================================================
    UdpSocket::Status	UdpSocket::Bind( unsigned short _port, const IpAddress& _address )
    {
	    return mSocket.bind( _port, _address );
    }

	//========================================================================================================
	//========================================================================================================
    UdpSocket::Status UdpSocket::Receive( Packet& _packet,
                                          IpAddress& _remoteAddress,
                                          unsigned short& _remotePort )
	{ 
		const Status status = mSocket.receive( _packet.ToSfml(), _remoteAddress, _remotePort );
		
		// Read tag
		if( status == sf::UdpSocket::Done )
		{
			_packet >> _packet.mTag;
		}
		return status;
	}

    //========================================================================================================
    //========================================================================================================
    UdpSocket::Status UdpSocket::Send( Packet& _packet,
                                       const IpAddress& _remoteAddress,
                                       unsigned short _remotePort )
    {
	    return mSocket.send( _packet.ToSfml(), _remoteAddress, _remotePort );
    }
}