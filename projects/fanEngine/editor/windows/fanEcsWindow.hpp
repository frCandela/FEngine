#pragma once

#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	// displays the state of the ecs world ( entities, components, memory consumption etc. )
	//========================================================================================================
	class EcsWindow : public EditorWindow
	{
	public:
		EcsWindow();

	protected:
		void OnGui( EcsWorld& _world ) override;
	};
}