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
		SingletonsWindow( EcsWorld& _world );
		
	protected:
		void OnGui() override;

	private:
		EcsWorld* m_world;
	};
}