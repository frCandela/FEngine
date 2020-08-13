#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
    struct UITransform;

	//========================================================================================================
	//========================================================================================================
	struct UIAlign : public EcsComponent
	{
		ECS_COMPONENT( UIAlign )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

        enum AlignCorner    : int { TopLeft, TopRight, BottomLeft, BottomRight };
        enum AlignDirection : int { Horizontal, Vertical, HorizontalVertical   };
        enum UnitType       : int { Ratio, Pixels   };

        ComponentPtr<UITransform>   mParent;
        AlignCorner                 mCorner;
        AlignDirection              mDirection;
        UnitType                    mUnitType;
		glm::vec2                   mOffset; // in pixel or in size ratio
	};
}