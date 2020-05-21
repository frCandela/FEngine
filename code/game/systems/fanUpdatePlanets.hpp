#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// moves the planets in circles around their origin ( zero for now )
	//==============================================================================================================================================================
	struct S_MovePlanets : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// generates a light mesh from the planets positions & radius
	//==============================================================================================================================================================
	struct S_GenerateLightMesh : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}