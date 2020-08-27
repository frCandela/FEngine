#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	// allows rendering of a mesh
	// needs a transform and a material
	//========================================================================================================
	struct MeshRenderer : public EcsComponent
	{
		ECS_COMPONENT( MeshRenderer )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		MeshPtr mMesh;
	};
}