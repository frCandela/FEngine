#include "engine/physics/fanSphereCollider.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SphereCollider::SetInfo( EcsComponentInfo& _info )
    {
        _info.load = &SphereCollider::Load;
        _info.save = &SphereCollider::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SphereCollider::Init( EcsWorld&, EcsEntity, EcsComponent& _component )
    {
        // clear
        SphereCollider& collider = static_cast<SphereCollider&>( _component );
        collider.mRadius = FIXED( 0.5 );
        collider.mOffset = Vector3::sZero;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SphereCollider::Save( const EcsComponent& _component, Json& _json )
    {
        const SphereCollider& collider = static_cast<const SphereCollider&>( _component );
        Serializable::SaveFixed( _json, "radius", collider.mRadius );
        Serializable::SaveVec3( _json, "offset", collider.mOffset );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SphereCollider::Load( EcsComponent& _component, const Json& _json )
    {
        SphereCollider& collider = static_cast<SphereCollider&>( _component );
        Serializable::LoadFixed( _json, "radius", collider.mRadius );
        Serializable::LoadVec3( _json, "offset", collider.mOffset );
    }
}

