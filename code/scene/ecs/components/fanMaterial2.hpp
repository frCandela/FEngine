#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Material2 : public ecComponent
	{
		DECLARE_COMPONENT( Material2 )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _material );
		static void Save( const ecComponent& _material, Json& _json );
		static void Load( ecComponent& _material, const Json& _json );

		TexturePtr	texture;
		uint32_t	shininess;
		Color		color;
	};
	static constexpr size_t sizeof_material2 = sizeof( Material2 );
}