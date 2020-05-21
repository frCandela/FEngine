#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Moves particles, kill them when they are out of time
	//==============================================================================================================================================================
	struct S_UpdateParticles : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}