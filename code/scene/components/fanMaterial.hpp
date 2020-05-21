#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Rendering parameters form a mesh renderer
	//==============================================================================================================================================================
	struct Material : public EcsComponent
	{
		ECS_COMPONENT( Material )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		TexturePtr	texture;
		uint32_t	shininess;
		Color		color;
	};
	static constexpr size_t sizeof_material2 = sizeof( Material );
}