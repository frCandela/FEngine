#pragma once

#include "editor/windows/fanEditorWindow.hpp"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// shows the singletons from the game ecs
	//================================================================================================================================
	class SingletonsWindow : public EditorWindow
	{
	public:
		SingletonsWindow();
		
	protected:
		void OnGui( EcsWorld& _world ) override;
	};
}