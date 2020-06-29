#pragma once

#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// diplays the state of the ecs world ( entities, components, memory consuption etc. )
	//================================================================================================================================
	class EcsWindow : public EditorWindow
	{
	public:
		EcsWindow();

	protected:
		void OnGui( EcsWorld& _world ) override;
	};
}