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
		EcsWindow( EcsWorld& _world );

	protected:
		void OnGui() override;

	private:
		EcsWorld* m_world = nullptr;
	};

}