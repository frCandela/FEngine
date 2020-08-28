#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanFollowTransform.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	// setups the FollowTransforms offsets @todo remove this ( useless ? )
	//========================================================================================================
	struct SInitFollowTransforms : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Transform>() | _world.GetSignature<FollowTransform>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			auto transformIt = _view.begin<Transform>();
			auto followTransformIt = _view.begin<FollowTransform>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++followTransformIt )
			{
				const Transform& follow = *transformIt;
				FollowTransform& followTransform = *followTransformIt;

				if( followTransform.mTargetTransform != nullptr )
				{
					Transform& target = *followTransform.mTargetTransform;
					followTransform.mLocalTransform =
					        SInitFollowTransforms::GetLocalTransform( target.mTransform, follow.mTransform );
				}
			}
		}

		static btTransform GetLocalTransform( const btTransform& _target, const btTransform& _follower )
		{
			return _target.inverse() * _follower;
		}
	};

	//========================================================================================================
	// moves the 3D FollowTransforms after their targets
	//========================================================================================================
	struct SMoveFollowTransforms : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Transform>() | _world.GetSignature<FollowTransform>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			auto transformIt = _view.begin<Transform>();
			auto followTransformIt = _view.begin<FollowTransform>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++followTransformIt )
			{
				Transform& follow = *transformIt;
				const FollowTransform& followTransform = *followTransformIt;

				if( followTransform.mLocked && followTransform.mTargetTransform != nullptr )
				{
					Transform& target = *followTransform.mTargetTransform;
					follow.mTransform = target.mTransform * followTransform.mLocalTransform;
				}
			}
		}
	};
}