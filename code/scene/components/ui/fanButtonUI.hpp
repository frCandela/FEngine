#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanSlot.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct Button : public EcsComponent
	{
		ECS_COMPONENT( Button )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		Color   mColorHovered;
        Color   mColorPressed;
		bool    mIsHovered;
        bool    mIsPressed;

        Signal<> mPressed;
        SlotPtr mSlotPtr;
	};
}