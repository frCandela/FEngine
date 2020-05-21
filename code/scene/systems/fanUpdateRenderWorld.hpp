#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Update the render world rendered meshes
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldModels : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );		
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Update the render world rendered ui meshes
	//==============================================================================================================================================================
	struct 	S_UpdateRenderWorldUI : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Update the render world point lights
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldPointLights : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Update the render world directional lights
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldDirectionalLights : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};
}