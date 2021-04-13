#include "engine/physics/fanFxBoxCollider.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxBoxCollider::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &FxBoxCollider::Load;
        _info.save = &FxBoxCollider::Save;
    }

    //========================================================================================================
    //========================================================================================================
    void FxBoxCollider::Init( EcsWorld&, EcsEntity, EcsComponent& _component )
    {
        // clear
        FxBoxCollider& collider = static_cast<FxBoxCollider&>( _component );
        collider.mHalfExtents = FIXED( 0.5 ) * Vector3::sOne;
    }

    //========================================================================================================
    //========================================================================================================
    void FxBoxCollider::Save( const EcsComponent& _component, Json& _json )
    {
        const FxBoxCollider& collider = static_cast<const FxBoxCollider&>( _component );
        Serializable::SaveVec3( _json, "half_extents", collider.mHalfExtents );
    }

    //========================================================================================================
    //========================================================================================================
    void FxBoxCollider::Load( EcsComponent& _component, const Json& _json )
    {
        FxBoxCollider& collider = static_cast<FxBoxCollider&>( _component );
        Serializable::LoadVec3( _json, "half_extents", collider.mHalfExtents );
    }
}

