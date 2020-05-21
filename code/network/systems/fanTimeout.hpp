#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_DetectHostTimout : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_ProcessTimedOutPackets : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};
}