#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Update the render world rendered meshes
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldModels : System
	{
		static Signature GetSignature( const EcsWorld& _world );		
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Update the render world rendered ui meshes
	//==============================================================================================================================================================
	struct 	S_UpdateRenderWorldUI : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Update the render world point lights
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldPointLights : System
	{
		static Signature GetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Update the render world directional lights
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldDirectionalLights : System
	{
		static Signature GetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
}