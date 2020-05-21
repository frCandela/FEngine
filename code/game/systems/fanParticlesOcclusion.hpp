#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// removes the particles that are not in the sunlight
	//==============================================================================================================================================================
	struct S_ParticlesOcclusion : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}