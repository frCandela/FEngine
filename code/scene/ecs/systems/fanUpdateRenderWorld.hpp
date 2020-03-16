#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldModels : System
	{
		static Signature GetSignature( const EcsWorld& _world );		
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct 	S_UpdateRenderWorldUI : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldPointLights : System
	{
		static Signature GetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldDirectionalLights : System
	{
		static Signature GetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );
	};
}