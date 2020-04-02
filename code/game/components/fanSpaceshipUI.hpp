#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/ui/fanProgressBar.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/ui/fanTransformUI.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class SpaceshipUI : public Component
	{
		DECLARE_COMPONENT( SpaceshipUI )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		glm::vec2 uiOffset;
		ComponentPtr<TransformUI> uiRootTransform;
		ComponentPtr<ProgressBar> healthProgress;
		ComponentPtr<ProgressBar> energyProgress;
		ComponentPtr<ProgressBar> signalProgress;
		ComponentPtr<UIRenderer>  signalRenderer;
	};
}