#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
    struct UITransform;

	//========================================================================================================
	//========================================================================================================
	struct UILayout : public EcsComponent
	{
		ECS_COMPONENT( UILayout )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		enum Type : int { Horizontal, Vertical};

        Type mType;
        int  mGap;
		std::vector< ComponentPtr<UITransform> > mTransforms;

		static ComponentPtr<UITransform>& CreateItem( EcsWorld& _world, UILayout& _layout );
	};
}