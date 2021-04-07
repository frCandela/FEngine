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
            auto transformIt = _view.begin<FxTransform>();
            auto rbIt = _view.begin<FxRigidbody>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++rbIt )
            {
                FxTransform& transform = *transformIt;
                FxRigidbody& rb = *rbIt;

                transform.mPosition += rb.mVelocity * _delta;
                transform.mPosition += FIXED(0.5) * rb.mAcceleration * _delta * _delta;

                Vector3 resultingAcceleration = rb.mAcceleration;
                resultingAcceleration += rb.mInverseMass * rb.mForcesAccumulator;
                rb.mForcesAccumulator = Vector3::sZero;

                rb.mVelocity += resultingAcceleration * _delta;
                rb.mVelocity *= _physicsWorld.mDamping;
            }
		}
	};
}