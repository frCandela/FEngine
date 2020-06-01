#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Sends packet for each client connection
	//==============================================================================================================================================================
	struct S_ClientSend : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// Receives packets for each client connection
	//==============================================================================================================================================================
	struct S_ClientReceive : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}