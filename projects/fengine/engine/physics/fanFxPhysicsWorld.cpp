#include "engine/physics//fanFxPhysicsWorld.hpp"

#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &FxPhysicsWorld::Save;
        _info.load = &FxPhysicsWorld::Load;
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        FxPhysicsWorld& physicsWorld = static_cast<FxPhysicsWorld&>( _component );
        physicsWorld.mGravity        = Vector3( 0, -10, 0 );
        physicsWorld.mLinearDamping  = FIXED( 0.995 );
        physicsWorld.mAngularDamping = FIXED( 0.995 );
        physicsWorld.mContactSolver = ContactSolver();
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::Save( const EcsSingleton& _component, Json& _json )
    {
        const FxPhysicsWorld& physicsWorld = static_cast<const FxPhysicsWorld&>( _component );
        Serializable::SaveVec3( _json, "gravity", physicsWorld.mGravity );
        Serializable::SaveFixed( _json, "damping", physicsWorld.mLinearDamping );
    }

    //========================================================================================================
    //========================================================================================================
    void FxPhysicsWorld::Load( EcsSingleton& _component, const Json& _json )
    {
        FxPhysicsWorld& physicsWorld = static_cast<FxPhysicsWorld&>( _component );
        Serializable::LoadVec3( _json, "gravity", physicsWorld.mGravity );
        Serializable::LoadFixed( _json, "damping", physicsWorld.mLinearDamping );
    }
}