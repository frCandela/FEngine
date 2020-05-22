#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Spawns the client spaceship at proper timing
	//==============================================================================================================================================================
	struct S_ClientSpawnSpaceship : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};	
	
	//==============================================================================================================================================================
	// Save the player state ( transform & rigidbody ) for detecting desync with the server simulation
	//==============================================================================================================================================================
	struct S_ClientSaveState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// Save the player input for sending over the network and making rollbacks in case of desync
	//==============================================================================================================================================================
	struct S_ClientSaveInput : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// Replicates components, singleton components & runs RPC
	//==============================================================================================================================================================
	struct S_ClientRunReplication : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// Detect server timeout on all clients connections
	//==============================================================================================================================================================
	struct S_ClientDetectServerTimeout : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}