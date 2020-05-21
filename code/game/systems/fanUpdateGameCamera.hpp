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
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}