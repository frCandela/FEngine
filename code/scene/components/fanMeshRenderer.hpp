#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// allows rendering of a mesh
	// needs a transform and a material
	//==============================================================================================================================================================
	struct MeshRenderer : public Component
	{
		DECLARE_COMPONENT( MeshRenderer )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		MeshPtr mesh;
		int renderID = -1;
	};
	static constexpr size_t sizeof_meshRenderer = sizeof( MeshRenderer );
}