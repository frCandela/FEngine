#include "scene/systems/fanUpdateTransforms.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanFollowTransform.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_InitFollowTransforms::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>() | _world.GetSignature<FollowTransform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_InitFollowTransforms::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			FollowTransform& followTransform = _world.GetComponent<FollowTransform>( entityID );
			if( followTransform.targetTransform != nullptr )
			{
				Transform& follow = _world.GetComponent<Transform>( entityID );
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
	Signature S_MoveFollowTransforms::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>() | _world.GetSignature<FollowTransform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_MoveFollowTransforms::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			FollowTransform& followTransform = _world.GetComponent<FollowTransform>( entityID );
			if( followTransform.locked && followTransform.targetTransform != nullptr )
			{
				Transform& follow = _world.GetComponent<Transform>( entityID );
				Transform& target = *followTransform.targetTransform;

				follow.transform = target.transform * followTransform.localTransform;
			}
		}
	}
}