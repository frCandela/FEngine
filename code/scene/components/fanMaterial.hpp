#pragma  once

#include "ecs/fanComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Rendering parameters form a mesh renderer
	//==============================================================================================================================================================
	struct Material : public Component
	{
		DECLARE_COMPONENT( Material )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		TexturePtr	texture;
		uint32_t	shininess;
		Color		color;
	};
	static constexpr size_t sizeof_material2 = sizeof( Material );
}