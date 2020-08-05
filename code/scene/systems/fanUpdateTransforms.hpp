#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/components/ui/fanTransformUI.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// setups the FollowTransforms offsets @todo remove this ( useless ? )
	//==============================================================================================================================================================
	struct S_InitFollowTransforms : EcsSystem
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

				if( followTransform.targetTransform != nullptr )
				{
					Transform& target = *followTransform.targetTransform;
					followTransform.localTransform = S_InitFollowTransforms::GetLocalTransform( target.transform, follow.transform );
				}
			}
		}

		static btTransform GetLocalTransform( const btTransform& _target, const btTransform& _follower )
		{
			return _target.inverse() * _follower;
		}
	};

	//==============================================================================================================================================================
	// moves the 3D FollowTransforms after their targets
	//==============================================================================================================================================================
	struct S_MoveFollowTransforms : EcsSystem
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

				if( followTransform.locked && followTransform.targetTransform != nullptr )
				{
					Transform& target = *followTransform.targetTransform;
					follow.transform = target.transform * followTransform.localTransform;
				}
			}
		}
	};

	//==============================================================================================================================================================
	// moves the 2D FollowTransforms after their targets
	//==============================================================================================================================================================
	struct SMoveFollowTransformsUI : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<TransformUI>() | _world.GetSignature<FollowTransformUI>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			auto transformUiIt = _view.begin<TransformUI>();
			auto followTransformIt = _view.begin<FollowTransformUI>();
			for( ; transformUiIt != _view.end<TransformUI>(); ++transformUiIt, ++followTransformIt )
			{
				TransformUI& follow = *transformUiIt;
				const FollowTransformUI& followTransform = *followTransformIt;

				if( followTransform.locked && followTransform.targetTransform != nullptr )
				{
					TransformUI& target = *followTransform.targetTransform;
					follow.mPosition = target.mPosition + followTransform.offset;
				}
			}
		}
	};
}