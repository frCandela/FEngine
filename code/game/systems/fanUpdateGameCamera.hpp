#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// moves the camera above the players spaceships
	//==============================================================================================================================================================
	struct S_UpdateGameCamera : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}