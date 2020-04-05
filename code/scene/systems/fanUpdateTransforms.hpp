#include "ecs/fanSystem.hpp"

#include "scene/components/fanTransform.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// setups the FollowTransforms offsets @todo remove this ( useless ? )
	//==============================================================================================================================================================
	struct S_InitFollowTransforms : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );

		static btTransform GetLocalTransform( const btTransform& _target, const btTransform& _follower );
	};

	//==============================================================================================================================================================
	// moves the 3D FollowTransforms after their targets
	//==============================================================================================================================================================
	struct S_MoveFollowTransforms : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// moves the 2D FollowTransforms after their targets
	//==============================================================================================================================================================
	struct S_MoveFollowTransformsUI : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
}