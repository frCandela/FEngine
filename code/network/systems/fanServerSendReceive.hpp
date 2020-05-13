#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Sends packets to all hosts
	//==============================================================================================================================================================
	struct S_ServerSend : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};	
	
	//==============================================================================================================================================================
	// Receives packets from all hosts
	//==============================================================================================================================================================
	struct S_ServerReceive : System
	{
		static void Run( EcsWorld& _world );
	};
}