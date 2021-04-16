#include "core/ecs/fanEcsSystem.hpp"
#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct SIntegrateFxRigidbodies : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<FxRigidbody>() | _world.GetSignature<FxTransform>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const Fixed _delta, FxPhysicsWorld& _physicsWorld )
		{
		    if( _delta == 0 ){ return; }

            auto transformIt = _view.begin<FxTransform>();
            auto rbIt = _view.begin<FxRigidbody>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++rbIt )
            {
                FxTransform& transform = *transformIt;
                FxRigidbody& rb = *rbIt;

                if( rb.mInverseMass == 0 )
                {
                    rb.mVelocity = rb.mRotation = Vector3::sZero;
                    continue;
                }

                rb.CalculateDerivedData( transform );

                // calculates linear/angular accelerations from forces input
                Vector3 resultingLinearAcceleration = rb.mAcceleration;
                resultingLinearAcceleration += rb.mInverseMass * rb.mForcesAccumulator;
                Vector3 resultingAngularAcceleration = rb.mInverseInertiaTensorWorld * rb.mTorqueAccumulator;

                rb.mVelocity += _delta * resultingLinearAcceleration;
                rb.mRotation += _delta * resultingAngularAcceleration;

                transform.mPosition += _delta * rb.mVelocity;
                transform.mRotation += FIXED( 0.5 ) * _delta * Quaternion( 0, rb.mRotation ) * transform.mRotation ;
                transform.mRotation.Normalize();

                rb.mVelocity *= _physicsWorld.mLinearDamping;
                rb.mRotation *= _physicsWorld.mAngularDamping;

                rb.ClearAccumulators();
            }
        }
	};
}