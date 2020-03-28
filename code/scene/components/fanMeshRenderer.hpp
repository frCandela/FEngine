#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;
	class Mesh;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct MeshRenderer : public Component
	{
		DECLARE_COMPONENT( MeshRenderer )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _meshRenderer );
		static void Save( const Component& _meshRenderer, Json& _json );
		static void Load( Component& _meshRenderer, const Json& _json );

		MeshPtr mesh;
		int renderID = -1;
	};
	static constexpr size_t sizeof_meshRenderer = sizeof( MeshRenderer );
}