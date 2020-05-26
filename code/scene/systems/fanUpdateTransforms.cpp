#include "scene/systems/fanUpdateTransforms.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/components/ui/fanTransformUI.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_InitFollowTransforms::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>() | _world.GetSignature<FollowTransform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_InitFollowTransforms::Run( EcsWorld& _world, const EcsView& _view ) 
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

	//================================================================================================================================
	//================================================================================================================================
	btTransform S_InitFollowTransforms::GetLocalTransform( const btTransform& _target, const btTransform& _follower )
	{	
		return _target.inverse() * _follower;
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_MoveFollowTransforms::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>() | _world.GetSignature<FollowTransform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_MoveFollowTransforms::Run( EcsWorld& _world, const EcsView& _view ) 
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

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_MoveFollowTransformsUI::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<TransformUI>() | _world.GetSignature<FollowTransformUI>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_MoveFollowTransformsUI::Run( EcsWorld& _world, const EcsView& _view ) 
	{
		auto transformUiIt = _view.begin<TransformUI>();
		auto followTransformIt = _view.begin<FollowTransformUI>();
		for( ; transformUiIt != _view.end<TransformUI>(); ++transformUiIt, ++followTransformIt )
		{
			TransformUI& follow = *transformUiIt;
			const FollowTransformUI& followTransform = *followTransformIt;

			if( followTransform.locked && followTransform.targetTransform != nullptr )
			{
// 				TransformUI& target = *followTransform.targetTransform;
// 				follow.position = target.position + followTransform.offset;
			}
		}
	}
}