#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/ui/fanProgressBar.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/ui/fanTransformUI.hpp"

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

		glm::vec2 uiOffset;
		ComponentPtr<TransformUI> uiRootTransform;
		ComponentPtr<ProgressBar> healthProgress;
		ComponentPtr<ProgressBar> energyProgress;
		ComponentPtr<ProgressBar> signalProgress;
		ComponentPtr<UIRenderer>  signalRenderer;
	};
}