#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct UIMainMenu : public EcsComponent
	{
		ECS_COMPONENT( UIMainMenu )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		// slots
		static void OnShowMain( EcsComponent& _component );
        static void OnShowOptions( EcsComponent& _component );
        static void OnShowCredits( EcsComponent& _component );
        static void OnPlay( EcsComponent& _component );
        static void OnQuit( EcsComponent& _component );


        ComponentPtr<SceneNode> mPlayNode;
        ComponentPtr<SceneNode> mOptionsNode;
        ComponentPtr<SceneNode> mCreditsNode;
        ComponentPtr<SceneNode> mQuitNode;
	};
}