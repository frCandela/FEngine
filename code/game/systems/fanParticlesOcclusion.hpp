#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// removes the particles that are not in the sunlight
	//==============================================================================================================================================================
	struct S_ParticlesOcclusion : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}