#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Button : public EcsComponent
	{
		ECS_COMPONENT( Button )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		Color   mColor;
		bool    mIsHovered;

		/*ComponentPtr<TransformUI> targetUiTransform;
		float	progress;
		float	maxScale;*/

	};
}