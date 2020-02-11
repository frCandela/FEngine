#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================
	class EcsWindow : public EditorWindow
	{
	public:
		EcsWindow();

		void SetEcsManager( EcsManager* _ecsManager ) { m_ecsManager = _ecsManager; }

	protected:
		void OnGui() override;


	private:
		EcsManager* m_ecsManager = nullptr;
	};

}