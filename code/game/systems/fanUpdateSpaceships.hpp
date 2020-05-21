#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// moves the spaceships
	//==============================================================================================================================================================
	struct S_MoveSpaceships : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// Deals damage to the health entities that are in the sunlight when the sun is bursting
	//==============================================================================================================================================================
	struct S_EruptionDamage : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// checks if any player is death and makes an explosion
	//==============================================================================================================================================================
	struct S_PlayerDeath : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}