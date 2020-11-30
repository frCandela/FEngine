#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "engine/fanSceneResourcePtr.hpp"
#include "engine/components/ui/fanUIRenderer.hpp"
#include "engine/components/ui/fanUIProgressBar.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/ui/fanUITransform.hpp"

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