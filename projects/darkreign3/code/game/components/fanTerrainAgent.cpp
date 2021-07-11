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
        agent.mHeightOffset     = 0;
        agent.mMoveSpeed        = 10;
        agent.mRotationSpeed    = 180;
        agent.mRange            = 2;
        agent.mAlignWithTerrain = true;

        agent.mState             = State::Stay;
        agent.mTargetPosition    = Vector3::sZero;
        agent.mPositionOnTerrain = Vector3::sZero;
        agent.mTerrainNormal              = Vector3::sUp;
        agent.mSqrDistanceFromDestination = 0;
        agent.mForwardAngle               = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainAgent::Save( const EcsComponent& _component, Json& _json )
    {
        const TerrainAgent& agent = static_cast<const TerrainAgent&>( _component );
        Serializable::SaveFixed( _json, "height_offset", agent.mHeightOffset );
        Serializable::SaveFixed( _json, "move_speed", agent.mMoveSpeed );
        Serializable::SaveFixed( _json, "rotation_speed", agent.mRotationSpeed );
        Serializable::SaveFixed( _json, "range", agent.mRange );
        Serializable::SaveBool( _json, "align_with_terrain", agent.mAlignWithTerrain );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void TerrainAgent::Load( EcsComponent& _component, const Json& _json )
    {
        TerrainAgent& agent = static_cast<TerrainAgent&>( _component );
        Serializable::LoadFixed( _json, "height_offset", agent.mHeightOffset );
        Serializable::LoadFixed( _json, "move_speed", agent.mMoveSpeed );
        Serializable::LoadFixed( _json, "rotation_speed", agent.mRotationSpeed );
        Serializable::LoadFixed( _json, "range", agent.mRange );
        Serializable::LoadBool( _json, "align_with_terrain", agent.mAlignWithTerrain );
    }
}