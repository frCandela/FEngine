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
	void S_InitFollowTransforms::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities )
	{
		for( EcsEntity entity : _entities )
		{
			FollowTransform& followTransform = _world.GetComponent<FollowTransform>( entity );
			if( followTransform.targetTransform != nullptr )
			{
				Transform& follow = _world.GetComponent<Transform>( entity );
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
	void S_MoveFollowTransforms::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities )
	{
		for( EcsEntity entity : _entities )
		{
			FollowTransform& followTransform = _world.GetComponent<FollowTransform>( entity );
			if( followTransform.locked && followTransform.targetTransform != nullptr )
			{
				Transform& follow = _world.GetComponent<Transform>( entity );
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
	void S_MoveFollowTransformsUI::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities )
	{
		for( EcsEntity entity : _entities )
		{
			FollowTransformUI& followTransform = _world.GetComponent<FollowTransformUI>( entity );
			if( followTransform.locked && followTransform.targetTransform != nullptr )
			{
				TransformUI& follow = _world.GetComponent<TransformUI>( entity );
				TransformUI& target = *followTransform.targetTransform;

				follow.position = target.position + followTransform.offset;
			}
		}
	}
}