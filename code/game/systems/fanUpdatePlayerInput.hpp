#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;
	struct PlayerInput;

	//==============================================================================================================================================================
	// refresh player input data from the mouse & keyboard
	//==============================================================================================================================================================
	struct S_RefreshPlayerInput : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}