#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Uses the rigidbody bounds to set the entity bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromRigidbody : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
		
	//==============================================================================================================================================================
	// Uses the convex hull in the mesh renderer mesh to generate new bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromModel : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Uses a transform bounds to set the entity bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromTransform : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}