#include "scene/ecs/fanSystem.hpp"

#include "scene/ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Draw the bounds of all scene nodes 
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Draw the normals of all models 
	//==============================================================================================================================================================
	struct S_DrawDebugNormals : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the models in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugWireframe : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the models convex hull in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugHull : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the point lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugPointLights : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the directional lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugDirectionalLights : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities );
	};
}