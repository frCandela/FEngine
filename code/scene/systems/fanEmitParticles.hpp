#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Spawns particles for all particle emitters in the scene
	//==============================================================================================================================================================
	struct S_EmitParticles : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}