#include "ecs/fanEcsSystem.hpp"

#include "scene/components/fanTransform.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// setups the FollowTransforms offsets @todo remove this ( useless ? )
	//==============================================================================================================================================================
	struct S_InitFollowTransforms : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );

		static btTransform GetLocalTransform( const btTransform& _target, const btTransform& _follower );
	};

	//==============================================================================================================================================================
	// moves the 3D FollowTransforms after their targets
	//==============================================================================================================================================================
	struct S_MoveFollowTransforms : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// moves the 2D FollowTransforms after their targets
	//==============================================================================================================================================================
	struct S_MoveFollowTransformsUI : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};
}