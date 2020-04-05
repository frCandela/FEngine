#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class Renderer;

	//================================================================================================================================
	// shows data from the renderer
	// loaded mesh, textures, prefabs, lights & buffer sizes
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