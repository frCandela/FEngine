#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Sends packets to all hosts
	//==============================================================================================================================================================
	struct S_ServerSend : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};	
	
	//==============================================================================================================================================================
	// Receives packets from all hosts
	//==============================================================================================================================================================
	struct S_ServerReceive : EcsSystem
	{
		static void Run( EcsWorld& _world );
	};
}