#include "game/components/fanTerrainAgent.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainAgent::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &TerrainAgent::Load;
        _info.save = &TerrainAgent::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainAgent::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        TerrainAgent& agent = static_cast<TerrainAgent&>( _component );
        agent.mHeightOffset = 0;
        agent.mDestination = Vector3::sZero;
        agent.mMoveSpeed   = 10;
        agent.mRotationSpeed = 360;
        agent.mState       = State::Stay;
        agent.mTerrainNormal = Vector3::sUp;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainAgent::Save( const EcsComponent& _component, Json& _json )
    {
        const TerrainAgent& agent = static_cast<const TerrainAgent&>( _component );
        Serializable::SaveFixed( _json, "height_offset", agent.mHeightOffset );
        Serializable::SaveFixed( _json, "move_speed", agent.mMoveSpeed );
        Serializable::SaveFixed( _json, "rotation_speed", agent.mRotationSpeed );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainAgent::Load( EcsComponent& _component, const Json& _json )
    {
        TerrainAgent& agent = static_cast<TerrainAgent&>( _component );
        Serializable::LoadFixed( _json, "height_offset", agent.mHeightOffset );
        Serializable::LoadFixed( _json, "move_speed", agent.mMoveSpeed );
        Serializable::LoadFixed( _json, "rotation_speed", agent.mRotationSpeed );
    }
}