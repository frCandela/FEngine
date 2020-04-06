#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// enumerates over all rigidbody in the world, find the ones that have a collision shape and 
	// optionally a motion state and adds them to the physics world.
	// this can be called post scene load to register all new scene nodes that need it
	//==============================================================================================================================================================
	struct S_RegisterAllRigidbodies : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );		
	};

	//==============================================================================================================================================================
	// enumerates over all rigidbody in the world, and removes them from the physics world
	//==============================================================================================================================================================
	struct S_UnregisterAllRigidbodies : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
}