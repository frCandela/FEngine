#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Spawns the client spaceship at proper timing
	//==============================================================================================================================================================
	struct S_ClientSpawnSpaceship : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};	
	
	//==============================================================================================================================================================
	// Save the player state ( transform & rigidbody ) for detecting desync with the server simulation
	//==============================================================================================================================================================
	struct S_ClientSaveState : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Save the player input for sending over the network and making rollbacks in case of desync
	//==============================================================================================================================================================
	struct S_ClientSaveInput : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Replicates components, singleton components & runs RPC
	//==============================================================================================================================================================
	struct S_ClientRunReplication : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Detect server timeout on all clients connections
	//==============================================================================================================================================================
	struct S_ClientDetectServerTimeout : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}