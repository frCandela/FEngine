#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Renderer;

	//================================================================================================================================
	//================================================================================================================================
	class RenderWindow : public EditorWindow {
	public:
		RenderWindow( Renderer * _renderer );

	protected:
		Renderer * m_renderer;

		void OnGui() override;
	};
}