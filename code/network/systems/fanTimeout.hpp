#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_DetectHostTimout : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_ProcessTimedOutPackets : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};
}