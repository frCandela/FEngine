#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Updates network objects in preparation for sending it to all clients
	//==============================================================================================================================================================
	struct S_ClientNetworkUpdate : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};	
	
	//==============================================================================================================================================================
	// Updates network objects in preparation for sending it to all clients
	//==============================================================================================================================================================
	struct S_ClientNetworkSend : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}