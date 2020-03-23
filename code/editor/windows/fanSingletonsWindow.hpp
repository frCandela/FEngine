#pragma once

#include "editor/fanEditorPrecompiled.hpp"

#include "editor/windows/fanEditorWindow.hpp"
#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
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