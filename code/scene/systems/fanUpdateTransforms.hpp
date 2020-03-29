#include "ecs/fanSystem.hpp"

#include "scene/components/fanTransform.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_InitFollowTransforms : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );

		static btTransform GetLocalTransform( const btTransform& _target, const btTransform& _follower );
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_MoveFollowTransforms : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
}