#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanUIMesh.hpp"

namespace fan
{
	struct ComponentInfo;
	class UIMesh;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UIRenderer : public ecComponent
	{
		DECLARE_COMPONENT( UIRenderer )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _component );
		static void Save( const ecComponent& _component, Json& _json );
		static void Load( ecComponent& _component, const Json& _json );

		UIMesh		uiMesh;
		Color		color;
		TexturePtr	texture;

		glm::ivec2	GetTextureSize() const;
		Texture*	GetTexture() const { return *texture; }
	};
	static constexpr size_t sizeof_UIRenderer = sizeof( UIRenderer );
}