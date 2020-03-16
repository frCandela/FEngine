#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Generate a mesh for all particles in the world
	//==============================================================================================================================================================
	struct S_GenerateParticles : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}