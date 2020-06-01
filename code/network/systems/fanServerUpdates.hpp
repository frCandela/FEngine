#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Runs the spawn ship RPC on synchronized hosts
	//==============================================================================================================================================================
	struct S_HostSpawnShip : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// When host frame index is offset too far from the server frame index, syncs it brutally
	//==============================================================================================================================================================
	struct S_HostSyncFrame : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// Save the host state ( transform & rigidbody ) for detecting desync with the host simulation
	//==============================================================================================================================================================
	struct S_HostSaveState : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// Updates the host input with the one received over network
	//==============================================================================================================================================================
	struct S_HostUpdateInput : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}