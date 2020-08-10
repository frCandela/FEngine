#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/ui/fanUIProgressBar.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/ui/fanUITransform.hpp"

namespace fan
{
	//================================================================================================================================
	// the ui flating on top of a spaceship
	//================================================================================================================================
	class SpaceshipUI : public EcsComponent
	{
		ECS_COMPONENT( SpaceshipUI )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		glm::vec2                   uiOffset;
		ComponentPtr<UITransform>   uiRootTransform;
		ComponentPtr<UIProgressBar> healthProgress;
		ComponentPtr<UIProgressBar> energyProgress;
		ComponentPtr<UIProgressBar> signalProgress;
		ComponentPtr<UIRenderer>    signalRenderer;
	};
}