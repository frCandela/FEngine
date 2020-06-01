#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Update the render world rendered meshes
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldModels : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );		
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};

	//==============================================================================================================================================================
	// Update the render world rendered ui meshes
	//==============================================================================================================================================================
	struct 	S_UpdateRenderWorldUI : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};

	//==============================================================================================================================================================
	// Update the render world point lights
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldPointLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};

	//==============================================================================================================================================================
	// Update the render world directional lights
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldDirectionalLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );

		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};
}