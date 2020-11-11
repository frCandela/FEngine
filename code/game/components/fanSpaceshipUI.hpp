#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/ui/fanUIProgressBar.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/ui/fanUITransform.hpp"

namespace fan
{
	//========================================================================================================
	// ui bars on top of a spaceship
	//========================================================================================================
	class SpaceshipUI : public EcsComponent
	{
		ECS_COMPONENT( SpaceshipUI )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		glm::vec2                   mUIOffset;
		ComponentPtr<UITransform>   mUIRootTransform;
		ComponentPtr<UIProgressBar> mHealthProgress;
		ComponentPtr<UIProgressBar> mEnergyProgress;
		ComponentPtr<UIProgressBar> mSignalProgress;
		ComponentPtr<UIRenderer>    mSignalRenderer;
	};
}