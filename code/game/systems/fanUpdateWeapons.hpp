#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// fires the weapons :
	// creates bullets at the proper position & speed depending on player input
	//==============================================================================================================================================================
	struct S_FireWeapons : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}