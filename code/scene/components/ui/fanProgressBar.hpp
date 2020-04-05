#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class ProgressBar : public Component
	{
		DECLARE_COMPONENT( ProgressBar )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		ComponentPtr<TransformUI> targetUiTransform;
		float	progress;
		float	maxScale;

		void SetProgress( const float _progress );
	};
	static constexpr size_t sizeof_progressBar = sizeof( ProgressBar );
}