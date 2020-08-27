#pragma once

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	struct EditorSelection;

	//========================================================================================================
	// copy paste editor callbacks
	//========================================================================================================
	struct EditorCopyPaste : EcsSingleton
	{
		ECS_SINGLETON( EditorCopyPaste );
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		// editor callbacks
		void OnCopy();
		void OnPaste();

		EditorSelection* mSelection;
	};
}