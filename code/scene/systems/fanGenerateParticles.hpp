#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Generate a mesh for all particles in the world
	//==============================================================================================================================================================
	struct S_GenerateParticles : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}