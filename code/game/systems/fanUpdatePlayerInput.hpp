#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;
	struct PlayerInput;

	//==============================================================================================================================================================
	// refresh player input data from the mouse & keyboard
	//==============================================================================================================================================================
	struct S_RefreshPlayerInput : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}