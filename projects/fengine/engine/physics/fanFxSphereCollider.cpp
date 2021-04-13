#include "engine/physics/fanFxSphereCollider.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxSphereCollider::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &FxSphereCollider::Load;
        _info.save = &FxSphereCollider::Save;
    }

    //========================================================================================================
    //========================================================================================================
    void FxSphereCollider::Init( EcsWorld&, EcsEntity, EcsComponent& _component )
    {
        // clear
        FxSphereCollider& collider = static_cast<FxSphereCollider&>( _component );
        collider.mRadius = FIXED( 0.5 );
    }

    //========================================================================================================
    //========================================================================================================
    void FxSphereCollider::Save( const EcsComponent& _component, Json& _json )
    {
        const FxSphereCollider& collider = static_cast<const FxSphereCollider&>( _component );
        Serializable::SaveFixed( _json, "radius", collider.mRadius );
    }

    //========================================================================================================
    //========================================================================================================
    void FxSphereCollider::Load( EcsComponent& _component, const Json& _json )
    {
        FxSphereCollider& collider = static_cast<FxSphereCollider&>( _component );
        Serializable::LoadFixed( _json, "radius", collider.mRadius );
    }
}

