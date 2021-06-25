#include "game/units/fanJudas.hpp"
#include "engine/fanEngineSerializable.hpp"


namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &Judas::Load;
        _info.save = &Judas::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Judas& judas = static_cast<Judas&>( _component );
        judas.mAnimIdle = nullptr;
        judas.mAnimWalk = nullptr;
        judas.mAnimRun = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::Save( const EcsComponent& _component, Json& _json )
    {
        const Judas& judas = static_cast<const Judas&>( _component );
        Serializable::SaveResourcePtr( _json, "anim_idle", judas.mAnimIdle );
        Serializable::SaveResourcePtr( _json, "anim_walk", judas.mAnimWalk );
        Serializable::SaveResourcePtr( _json, "anim_run", judas.mAnimRun );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Judas::Load( EcsComponent& _component, const Json& _json )
    {
        Judas& judas = static_cast<Judas&>( _component );
        Serializable::LoadResourcePtr( _json, "anim_idle", judas.mAnimIdle );
        Serializable::LoadResourcePtr( _json, "anim_walk", judas.mAnimWalk );
        Serializable::LoadResourcePtr( _json, "anim_run", judas.mAnimRun );
    }
}