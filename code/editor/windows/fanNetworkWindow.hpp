#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{

	struct Scene;

	//================================================================================================================================
	// @wip
	//================================================================================================================================
	class NetworkWindow : public EditorWindow
	{
	public:

		NetworkWindow( Scene& _scene );
	protected:
		void OnGui() override;

	private:
		Scene* m_scene = nullptr;
	};
}