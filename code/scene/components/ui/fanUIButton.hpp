#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanSlot.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "core/fanColor.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct UIButton : public EcsComponent
	{
		ECS_COMPONENT( UIButton )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		bool        mIsHovered;
        bool        mIsPressed;
        Color       mColorNormal;
        Color       mColorHovered;
        TexturePtr  mImageNormal;
        TexturePtr  mImagePressed;

        Signal<> mPressed;
        SlotPtr  mSlotPtr;
	};
}