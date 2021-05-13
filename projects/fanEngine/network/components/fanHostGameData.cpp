#include "network/components/fanHostGameData.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void HostGameData::SetInfo( EcsComponentInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void HostGameData::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        HostGameData& hostGameData = static_cast<HostGameData&>( _component );
        hostGameData.mSpaceshipID          = 0;
        hostGameData.mSpaceshipHandle      = 0;
        hostGameData.mInputs               = std::queue<PacketInput::InputData>();
        hostGameData.mNextPlayerState      = PacketPlayerGameState();
        hostGameData.mNextPlayerStateFrame = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void HostGameData::ProcessPacket( PacketInput& _packet )
    {
        if( mInputs.empty() )
        {
            for( const PacketInput::InputData& inputData : _packet.mInputs )
            {
                mInputs.push( inputData );
            }
        }
        else
        {
            for( const PacketInput::InputData& inputData : _packet.mInputs )
            {
                const FrameIndex mostRecentFrame = mInputs.back().mFrameIndex;
                //fanAssert( inputData.frameIndex <= mostRecentFrame + 1 );
                if( inputData.mFrameIndex == mostRecentFrame + 1 )
                {
                    mInputs.push( inputData );
                }
            }
        }
    }
}