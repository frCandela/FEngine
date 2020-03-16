#include "scene/ecs/fanSystem.hpp"

#include "scene/ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Spawns particles for all particle emitters in the scene
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}