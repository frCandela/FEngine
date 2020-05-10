#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Sends packet for each client connection
	//==============================================================================================================================================================
	struct S_ClientSend : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Receives packets for each client connection
	//==============================================================================================================================================================
	struct S_ClientReceive : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}