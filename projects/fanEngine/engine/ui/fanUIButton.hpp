#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "core/ecs/fanSlot.hpp"
#include "fanUITransform.hpp"
#include "engine/fanSceneResourcePtr.hpp"
#include "core/fanColor.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UIButton : public EcsComponent
    {
    ECS_COMPONENT( UIButton )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        bool       mIsHovered;
        bool       mIsPressed;
        Color      mColorNormal;
        Color      mColorHovered;
        TexturePtr mImageNormal;
        TexturePtr mImagePressed;

        Signal<> mPressed;
        SlotPtr  mSlotPtr;
    };
}