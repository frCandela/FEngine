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
		EcsWindow( EcsWorld& _world );

	protected:
		void OnGui() override;

	private:
		EcsWorld* m_world = nullptr;
	};
}