#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanMesh2D.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UIRenderer : public EcsComponent
	{
		ECS_COMPONENT( UIRenderer )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		Mesh2D*    mUiMesh;
		Color      color;
		TexturePtr texture;

		glm::ivec2	GetTextureSize() const;
		Texture*	GetTexture() const { return *texture; }
	};
	static constexpr size_t sizeof_UIRenderer = sizeof( UIRenderer );
}