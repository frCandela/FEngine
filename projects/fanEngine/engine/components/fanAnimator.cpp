#include "engine/components/fanAnimator.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Animator::SetInfo( EcsComponentInfo& _info )
    {
        _info.save = &Animator::Save;
        _info.load = &Animator::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Animator::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        Animator& animator = static_cast<Animator&>( _component );
        animator.mAnimation = {};
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Animator::Save( const EcsComponent& _component, Json& _json )
    {
        const Animator& animator = static_cast<const Animator&>( _component );
        Serializable::SaveResourcePtr( _json, "animation", animator.mAnimation.mData );
        Serializable::SaveBool( _json, "loop", animator.mLoop );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Animator::Load( EcsComponent& _component, const Json& _json )
    {
        Animator& animator = static_cast<Animator&>( _component );
        Serializable::LoadResourcePtr( _json, "animation", animator.mAnimation.mData );
        Serializable::LoadBool( _json, "loop", animator.mLoop );
    }
}