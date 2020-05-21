#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Draw the bounds of all scene nodes 
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw the normals of all models 
	//==============================================================================================================================================================
	struct S_DrawDebugNormals : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the models in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugWireframe : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the models convex hull in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugHull : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the point lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugPointLights : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the directional lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugDirectionalLights : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw physics shapes in wireframe (box, sphere, etc )
	//==============================================================================================================================================================
	struct S_DrawDebugCollisionShapes : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};
}