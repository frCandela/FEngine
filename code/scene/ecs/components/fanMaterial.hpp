#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Material : public Component
	{
		DECLARE_COMPONENT( Material )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _material );
		static void Save( const Component& _material, Json& _json );
		static void Load( Component& _material, const Json& _json );

		TexturePtr	texture;
		uint32_t	shininess;
		Color		color;
	};
	static constexpr size_t sizeof_material2 = sizeof( Material );
}