#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class ProgressBar : public EcsComponent
	{
		ECS_COMPONENT( ProgressBar )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		ComponentPtr<TransformUI> targetUiTransform;
		float	progress;
		float	maxScale;

		void SetProgress( const float _progress );
	};
	static constexpr size_t sizeof_progressBar = sizeof( ProgressBar );
}