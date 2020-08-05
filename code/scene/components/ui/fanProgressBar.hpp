#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct ProgressBar : public EcsComponent
	{
		ECS_COMPONENT( ProgressBar )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		ComponentPtr<TransformUI> mTargetTransform;
		float                     mProgress; // between 0-1
		int                       mMaxSize;  // in pixels

		void SetProgress( const float _progress );
	};
}