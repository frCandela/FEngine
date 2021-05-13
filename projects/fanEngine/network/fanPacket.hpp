#pragma once

#include <iostream>
#include <limits>
#include <type_traits>
#include "core/ecs/fanSignal.hpp"
#include "core/fanDebug.hpp"
#include "network/fanNetConfig.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    struct Client;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Packet
    {
        Packet() {} // for receiving only
        Packet( const PacketTag _tag );

        template< typename T >
        Packet& operator<<( T _value )
        {
            mPacket << _value;
            return *this;
        }

        template< typename T >
        Packet& operator>>( T& _value )
        {
            mPacket >> _value;
            return *this;
        }

        bool EndOfPacket() const { return mPacket.endOfPacket(); }
        sf::Packet& ToSfml() { return mPacket; }
        void Clear();
        PacketType ReadType();
        size_t GetSize() const { return mPacket.getDataSize(); }

        PacketTag         mTag;
        Signal<PacketTag> mOnFail;        // packet was dropped
        Signal<PacketTag> mOnSuccess;    // packet was received
        bool              mOnlyContainsAck = false;
    private:
        sf::Packet mPacket;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct PacketAck
    {
        void Write( Packet& _packet )
        {
            _packet << PacketTypeInt( PacketType::Ack );
            _packet << sf::Uint16( mTags.size() );
            for( PacketTag tag : mTags )
            {
                _packet << tag;
            }
        }
        void Read( Packet& _packet )
        {
            sf::Uint16 size;
            _packet >> size;
            mTags.resize( size );
            for( int i = 0; i < size; i++ )
            {
                _packet >> mTags[i];
            }
        }

        std::vector<PacketTag> mTags;
    };

    //==================================================================================================================================================================================================
    // server -> client then client -> server
    // Used to calculate a the connection RTT & frame index synch delta with the client
    //==================================================================================================================================================================================================
    struct PacketPing
    {
        void Read( Packet& _packet )
        {
            _packet >> mServerFrame;
            _packet >> mClientFrame;
            _packet >> mPreviousRtt;
        }

        void Write( Packet& _packet ) const
        {
            _packet << PacketTypeInt( PacketType::Ping );
            _packet << mServerFrame;
            _packet << mClientFrame;
            _packet << mPreviousRtt;
        }

        FrameIndex mServerFrame = 0; // frame index of the server when sending the packet
        FrameIndex mClientFrame = 0; // frame index of the client when sending back the packet
        float      mPreviousRtt; // client rtt from the previous ping
    };

    //==================================================================================================================================================================================================
    // client -> server
    // first packet send from the client to login into the server
    // server should respond with a LoggedIn packet
    //==================================================================================================================================================================================================
    struct PacketHello
    {
        void Write( Packet& _packet ) const
        {
            _packet << PacketTypeInt( PacketType::Hello );
            _packet << mName;
        }
        void Read( Packet& _packet )
        {
            _packet >> mName;
        }

        std::string mName = "";
    };

    //==================================================================================================================================================================================================
    // client -> server when the client disconnects
    // server -> client when the server shuts down
    //==================================================================================================================================================================================================
    struct PacketDisconnect
    {
        void Write( Packet& _packet ) const
        {
            _packet << PacketTypeInt( PacketType::Disconnect );
        }
        void Read( Packet& /*_packet*/ ) {}
    };

    //==================================================================================================================================================================================================
    // server -> client
    // Packet send from the server to the client after a Hello to acknowledge successful login
    //==================================================================================================================================================================================================
    struct PacketLoginSuccess
    {
        void Write( Packet& _packet ) const
        {
            _packet << PacketTypeInt( PacketType::LoggedIn );
            _packet << mPlayerId;
        }
        void Read( Packet& _packet )
        {
            _packet >> mPlayerId;
        }

        PlayerID mPlayerId = 0; // corresponds to the player persistent entity handle
    };

    //==================================================================================================================================================================================================
    // server->client  Replication packet for singleton components, components & RPC
    // serialized data is stored in a sf::Packet, it must be generated from the server replication manager
    //==================================================================================================================================================================================================
    struct PacketReplication
    {
        void Read( Packet& _packet );
        void Write( Packet& _packet ) const;

        enum class ReplicationType
        {
            SingletonComponent, Entity, RPC, Count
        };

        ReplicationType mReplicationType = ReplicationType::Count;
        sf::Packet      mPacketData;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct PacketInput
    {
        struct InputData
        {
            bool         mLeft : 1;
            bool         mRight : 1;
            bool         mForward : 1;
            bool         mBackward : 1;
            bool         mBoost : 1;
            bool         mFire : 1;
            sf::Vector2f mOrientation;
            FrameIndex   mFrameIndex;
        };

        std::vector<InputData> mInputs;

        void Read( Packet& _packet )
        {
            sf::Uint8 size;
            _packet >> size;

            mInputs.resize( size );
            for( int i = 0; i < size; i++ )
            {
                InputData& inputData = mInputs[i];
                _packet >> inputData.mFrameIndex;
                _packet >> inputData.mOrientation.x >> inputData.mOrientation.y;
                sf::Uint8 keyBits;
                _packet >> keyBits;

                inputData.mLeft     = keyBits & ( 1 << 0 );
                inputData.mRight    = keyBits & ( 1 << 1 );
                inputData.mForward  = keyBits & ( 1 << 2 );
                inputData.mBackward = keyBits & ( 1 << 3 );
                inputData.mBoost    = keyBits & ( 1 << 4 );
                inputData.mFire     = keyBits & ( 1 << 5 );
            }
        }

        void Write( Packet& _packet ) const
        {
            _packet << PacketTypeInt( PacketType::PlayerInput );
            _packet << sf::Uint8( mInputs.size() );

            for( int i = 0; i < (int)mInputs.size(); i++ )
            {
                const InputData& inputData = mInputs[i];
                const sf::Uint8 keyBits =
                                        inputData.mLeft << 0 |
                                        inputData.mRight << 1 |
                                        inputData.mForward << 2 |
                                        inputData.mBackward << 3 |
                                        inputData.mBoost << 4 |
                                        inputData.mFire << 5;

                _packet << inputData.mFrameIndex;
                _packet << inputData.mOrientation.x << inputData.mOrientation.y;
                _packet << keyBits;
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct PacketPlayerGameState
    {
        void Read( Packet& _packet )
        {
            _packet >> mFrameIndex;
            _packet >> mPosition[0].GetData() >> mPosition[1].GetData() >> mPosition[2].GetData();
            _packet >> mOrientation[0].GetData() >> mOrientation[1].GetData() >> mOrientation[2].GetData();
            _packet >> mVelocity[0].GetData() >> mVelocity[1].GetData() >> mVelocity[2].GetData();
            _packet >> mAngularVelocity[0].GetData() >> mAngularVelocity[1].GetData() >> mAngularVelocity[2].GetData();
        }

        void Write( Packet& _packet ) const
        {
            _packet << PacketTypeInt( PacketType::PlayerGameState );
            _packet << mFrameIndex;
            _packet << mPosition[0].GetData() << mPosition[1].GetData() << mPosition[2].GetData();
            _packet << mOrientation[0].GetData() << mOrientation[1].GetData() << mOrientation[2].GetData();
            _packet << mVelocity[0].GetData() << mVelocity[1].GetData() << mVelocity[2].GetData();
            _packet << mAngularVelocity[0].GetData() << mAngularVelocity[1].GetData() << mAngularVelocity[2].GetData();
        }

        bool operator==( const PacketPlayerGameState& _other ) const
        {
            return mFrameIndex == _other.mFrameIndex &&
                   Vector3::IsFuzzyZero( mPosition - _other.mPosition ) &&
                   Vector3::IsFuzzyZero( mOrientation - _other.mOrientation ) &&
                   Vector3::IsFuzzyZero( mVelocity - _other.mVelocity ) &&
                   Vector3::IsFuzzyZero( mAngularVelocity - _other.mAngularVelocity );
        }
        bool operator!=( const PacketPlayerGameState& _other ) const { return !( *this == _other ); }

        FrameIndex mFrameIndex = 0;            // the  frame index when creating state
        Vector3    mPosition;
        Vector3    mOrientation;
        Vector3    mVelocity;
        Vector3    mAngularVelocity;
    };
}