#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// updates solar panels charging rate depending on where they are in the sunlight
	//==============================================================================================================================================================
	struct S_UpdateSolarPannels : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// recharges batteries from solar panels
	//==============================================================================================================================================================
	struct S_RechargeBatteries : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}