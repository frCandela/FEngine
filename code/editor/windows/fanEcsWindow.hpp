#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	//================================================================================================================================
	class EcsWindow : public EditorWindow
	{
	public:
		EcsWindow();

		void SetEcsWorld( EcsWorld& _world ) { m_world = &_world; }

	protected:
		void OnGui() override;


	private:
		EcsWorld* m_world = nullptr;
	};

}