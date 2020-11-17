#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
    struct Mesh2D;
    class Font;

	//========================================================================================================
	//========================================================================================================
	struct UIText : public EcsComponent
	{
		ECS_COMPONENT( UIText )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		std::string mText;
		Mesh2D *    mMesh2D;
		FontPtr     mFontPtr;
		int         mSize;
	};
}