#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Uses the rigidbody bounds to set the entity bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromRigidbody : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
		
	//==============================================================================================================================================================
	// Uses the convex hull in the mesh renderer mesh to generate new bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromModel : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Uses a transform bounds to set the entity bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromTransform : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}