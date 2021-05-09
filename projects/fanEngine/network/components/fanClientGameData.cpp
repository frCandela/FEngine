#include "network/components/fanClientGameData.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientGameData::SetInfo( EcsComponentInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientGameData::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        ClientGameData& gameData = static_cast<ClientGameData&>( _component );
        gameData.sSpaceshipHandle     = 0;
        gameData.mFrameSynced         = false;
        gameData.mPreviousInputs      = std::deque<PacketInput::InputData>();    // clear
        gameData.mPreviousLocalStates = std::queue<PacketPlayerGameState>();    // clear
        gameData.mMaxInputSent        = 10;
        gameData.mSpaceshipSynced     = true;
        gameData.mLastServerState     = {};
        gameData.mPlayerId            = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientGameData::ProcessPacket( const PacketPlayerGameState& _packet )
    {
        mLastServerState = _packet; // @todo store multiple server states to allow deeper rollback

        // get the corresponding game state for the client
        while( !mPreviousLocalStates.empty() && mPreviousLocalStates.front().mFrameIndex < _packet.mFrameIndex )
        {
            mPreviousLocalStates.pop();
        }

        // compares the server state & client state to verify we are synchronized
        if( !mPreviousLocalStates.empty() && mPreviousLocalStates.front().mFrameIndex == _packet.mFrameIndex )
        {
            const PacketPlayerGameState& packetState = mPreviousLocalStates.front();
            mPreviousLocalStates.pop();

            if( packetState != _packet )
            {
                Debug::Warning() << "player is out of sync" << Debug::Endl();
                mSpaceshipSynced = false;

                if( packetState.mFrameIndex != _packet.mFrameIndex )
                {
                    Debug::Log() << "frame index difference: " << packetState.mFrameIndex << " " << _packet.mFrameIndex << Debug::Endl();
                }
                if( !Vector3::IsFuzzyZero( packetState.mPosition - _packet.mPosition ) )
                {
                    const Vector3 diff = packetState.mPosition - _packet.mPosition;
                    Debug::Log() << "position difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
                }
                if( !Vector3::IsFuzzyZero( packetState.mOrientation - _packet.mOrientation ) )
                {
                    const Vector3 diff = packetState.mOrientation - _packet.mOrientation;
                    Debug::Log() << "orientation difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
                }
                if( !Vector3::IsFuzzyZero( packetState.mVelocity - _packet.mVelocity ) )
                {
                    const Vector3 diff = packetState.mVelocity - _packet.mVelocity;
                    Debug::Log() << "velocity difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
                }
                if( !Vector3::IsFuzzyZero( packetState.mAngularVelocity - _packet.mAngularVelocity ) )
                {
                    const Vector3 diff = packetState.mAngularVelocity - _packet.mAngularVelocity;
                    Debug::Log() << "angular velocity difference: " << diff[0] << " " << diff[1] << " " << diff[2] << Debug::Endl();
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientGameData::Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet )
    {
        // calculates the number of inputs to send
        int numInputs = (int)mPreviousInputs.size();
        if( numInputs > mMaxInputSent )
        {
            numInputs = mMaxInputSent;
        }

        if( numInputs > 0 )
        {
            // registers packet success
            _packet.mOnSuccess.Connect( &ClientGameData::OnInputReceived, _world, _world.GetHandle( _entity ) );
            mInputsSent.push_front( { _packet.mTag, mPreviousInputs.front().mFrameIndex } );

            // generate & send inputs
            PacketInput packetInput;
            packetInput.mInputs.resize( numInputs );
            for( int i = 0; i < numInputs; i++ )
            {
                packetInput.mInputs[numInputs - i - 1] = *( mPreviousInputs.begin() + i );
            }
            packetInput.Write( _packet );
        }
    }

    //==================================================================================================================================================================================================
    // removes all inputs that have been received from the buffer
    //==================================================================================================================================================================================================
    void ClientGameData::OnInputReceived( PacketTag _tag )
    {
        while( !mInputsSent.empty() )
        {
            const InputSent inputSent = mInputsSent.back();
            if( inputSent.mTag < _tag ) // packets were lost but we don't care
            {
                mInputsSent.pop_back();
            }
            else if( inputSent.mTag == _tag )
            {
                mInputsSent.pop_back();
                // input packet was received, remove all corresponding inputs from the buffer
                while( !mPreviousInputs.empty() )
                {
                    const PacketInput::InputData packetInput = mPreviousInputs.back();
                    if( packetInput.mFrameIndex <= inputSent.mMostRecentFrame )
                    {
                        mPreviousInputs.pop_back();
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            }
            else
            {
                break;
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ClientGameData::OnShiftFrameIndex( const int /*_framesDelta*/ )
    {
        fanAssert( false );
        /* mPreviousLocalStates = std::queue<PacketPlayerGameState >(); // clear
         mFrameSynced         = true;

         if( std::abs( _framesDelta ) > Time::sMaxFrameDeltaBeforeShift )
         {
             Debug::Log() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
             mPreviousLocalStates = std::queue<PacketPlayerGameState >(); // clear
         }*/
    }
}