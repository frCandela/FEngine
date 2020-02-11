#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class Renderer;

	//================================================================================================================================
	//================================================================================================================================
	class RenderWindow : public EditorWindow
	{
	public:
		RenderWindow();
		void SetRenderer( Renderer* _renderer ) { m_renderer = _renderer; }

	protected:
		Renderer* m_renderer;

		void OnGui() override;
	};
}