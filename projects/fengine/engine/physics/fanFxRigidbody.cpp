#include "engine/physics/fanFxRigidbody.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &FxRigidbody::Load;
        _info.save = &FxRigidbody::Save;
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::Init( EcsWorld&, EcsEntity, EcsComponent& _component )
    {
        // clear
        FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
        rb.mVelocity     = Vector3::sZero;
        rb.mAcceleration = -10 * Vector3::sUp;
        rb.mForcesAccumulator = Vector3::sZero;
        rb.mInverseMass = 1;
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::Save( const EcsComponent& _component, Json& _json )
    {
        const FxRigidbody& rb = static_cast<const FxRigidbody&>( _component );
        Serializable::SaveFixed( _json, "inv_mass", rb.mInverseMass );
        Serializable::SaveVec3( _json, "velocity", rb.mVelocity );
        Serializable::SaveVec3( _json, "acceleration", rb.mAcceleration );
    }

    //========================================================================================================
    //========================================================================================================
    void FxRigidbody::Load( EcsComponent& _component, const Json& _json )
    {
        FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
        Serializable::LoadFixed( _json, "inv_mass", rb.mInverseMass );
        Serializable::LoadVec3( _json, "velocity", rb.mVelocity );
        Serializable::LoadVec3( _json, "acceleration", rb.mAcceleration );
    }
}

