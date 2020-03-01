#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class EntityWorld;

	//================================================================================================================================
	//================================================================================================================================
	class EcsWindow : public EditorWindow
	{
	public:
		EcsWindow();

		void SetEntityWorld( EntityWorld& _world ) { m_world = &_world; }

	protected:
		void OnGui() override;


	private:
		EntityWorld* m_world = nullptr;
	};

}