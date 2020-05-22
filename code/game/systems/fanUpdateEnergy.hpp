#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// updates solar panels charging rate depending on where they are in the sunlight
	//==============================================================================================================================================================
	struct S_UpdateSolarPannels : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};

	//==============================================================================================================================================================
	// recharges batteries from solar panels
	//==============================================================================================================================================================
	struct S_RechargeBatteries : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta );
	};
}