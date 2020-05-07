#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Updates network objects in preparation for sending it to all clients
	//==============================================================================================================================================================
	struct S_ServerUpdateHosts : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
	
	//==============================================================================================================================================================
	// Sends packets to all clients
	//==============================================================================================================================================================
	struct S_ServerNetworkSend : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
}