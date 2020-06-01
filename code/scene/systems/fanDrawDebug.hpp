#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Draw the bounds of all scene nodes 
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view );
	};

	//==============================================================================================================================================================
	// Draw the normals of all models 
	//==============================================================================================================================================================
	struct S_DrawDebugNormals : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};

	//==============================================================================================================================================================
	// Draw all the models in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugWireframe : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};

	//==============================================================================================================================================================
	// Draw all the models convex hull in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugHull : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;
	};


	struct Transform;
	struct PointLight;
	//==============================================================================================================================================================
	// Draw all the point lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugPointLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;

		static void DrawPointLight( const Transform& _transform, const PointLight& _light );
	};


	struct DirectionalLight;
	//==============================================================================================================================================================
	// Draw all the directional lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugDirectionalLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;

		static void DrawDirectionalLight( const Transform& _transform, const DirectionalLight& _light );
	};

	//==============================================================================================================================================================
	// Draw physics shapes in wireframe (box, sphere, etc )
	//==============================================================================================================================================================
	struct S_DrawDebugCollisionShapes : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const EcsView& _view ) ;

		static void DrawCollisionShape( EcsWorld& _world, EcsEntity _entity  );
	};
}