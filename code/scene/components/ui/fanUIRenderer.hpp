#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/resources/fanMesh2D.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct UIRenderer : public EcsComponent
	{
		ECS_COMPONENT( UIRenderer )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		Mesh2D*    mMesh2D;
		Color      mColor;
		TexturePtr mTexture;
		int        mDepth;

		glm::ivec2	GetTextureSize() const;
		Texture*	GetTexture() const { return *mTexture; }
	};
}