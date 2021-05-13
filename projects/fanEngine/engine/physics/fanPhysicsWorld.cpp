#include "engine/physics//fanPhysicsWorld.hpp"

#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PhysicsWorld::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &PhysicsWorld::Save;
        _info.load = &PhysicsWorld::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PhysicsWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );
        physicsWorld.mGravity        = Vector3( 0, -10, 0 );
        physicsWorld.mLinearDamping  = FIXED( 0.996 );
        physicsWorld.mAngularDamping = FIXED( 0.996 );
        physicsWorld.mContactSolver  = ContactSolver();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PhysicsWorld::Save( const EcsSingleton& _component, Json& _json )
    {
        const PhysicsWorld& physicsWorld = static_cast<const PhysicsWorld&>( _component );
        Serializable::SaveVec3( _json, "gravity", physicsWorld.mGravity );
        Serializable::SaveFixed( _json, "linear_damping", physicsWorld.mLinearDamping );
        Serializable::SaveFixed( _json, "angular_damping", physicsWorld.mAngularDamping );
        Serializable::SaveFixed( _json, "restitution", physicsWorld.mContactSolver.mRestitution );
        Serializable::SaveFixed( _json, "friction", physicsWorld.mContactSolver.mFriction );
        Serializable::SaveFixed( _json, "resting_velocity", physicsWorld.mContactSolver.mRestingVelocityLimit );
        Serializable::SaveInt( _json, "vel iterations", physicsWorld.mContactSolver.mMaxVelocityIterations );
        Serializable::SaveInt( _json, "pos iterations", physicsWorld.mContactSolver.mMaxPositionsIterations );
        Serializable::SaveFixed( _json, "alnlp", physicsWorld.mContactSolver.mAngularLimitNonLinearProjection );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PhysicsWorld::Load( EcsSingleton& _component, const Json& _json )
    {
        PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );
        Serializable::LoadVec3( _json, "gravity", physicsWorld.mGravity );
        Serializable::LoadFixed( _json, "linear_damping", physicsWorld.mLinearDamping );
        Serializable::LoadFixed( _json, "angular_damping", physicsWorld.mAngularDamping );
        Serializable::LoadFixed( _json, "restitution", physicsWorld.mContactSolver.mRestitution );
        Serializable::LoadFixed( _json, "friction", physicsWorld.mContactSolver.mFriction );
        Serializable::LoadFixed( _json, "resting_velocity", physicsWorld.mContactSolver.mRestingVelocityLimit );
        Serializable::LoadInt( _json, "vel iterations", physicsWorld.mContactSolver.mMaxVelocityIterations );
        Serializable::LoadInt( _json, "pos iterations", physicsWorld.mContactSolver.mMaxPositionsIterations );
        Serializable::LoadFixed( _json, "alnlp", physicsWorld.mContactSolver.mAngularLimitNonLinearProjection );
    }
}