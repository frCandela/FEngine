#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanUIMesh.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UIRenderer : public EcsComponent
	{
		ECS_COMPONENT( UIRenderer )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		UIMesh		uiMesh;
		Color		color;
		TexturePtr	texture;

		glm::ivec2	GetTextureSize() const;
		Texture*	GetTexture() const { return *texture; }
	};
	static constexpr size_t sizeof_UIRenderer = sizeof( UIRenderer );
}