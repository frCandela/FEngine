#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// allows rendering of a mesh
	// needs a transform and a material
	//==============================================================================================================================================================
	struct MeshRenderer : public EcsComponent
	{
		ECS_COMPONENT( MeshRenderer )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		MeshPtr mesh;
		int renderID = -1;
	};
	static constexpr size_t sizeof_meshRenderer = sizeof( MeshRenderer );
}