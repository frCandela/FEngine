#pragma once

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	struct EditorSelection;

	//========================================================================================================
	// copy paste editor callbacks
	// @todo make this a singleton component in the editor ecs world
	//========================================================================================================
	struct EditorCopyPaste : EcsSingleton
	{
		ECS_SINGLETON( EditorCopyPaste );
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		// editor callbacks
		void OnCopy();
		void OnPaste();

		EditorSelection* m_selection;
	};
}