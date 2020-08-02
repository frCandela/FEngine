#pragma once

#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	class Renderer;

	//========================================================================================================
	// shows data from the renderer
	// loaded mesh, textures, prefabs, lights & buffer sizes
	//========================================================================================================
	class RenderWindow : public EditorWindow
	{
	public:
		RenderWindow( Renderer& _renderer );

	protected:
		Renderer& m_renderer;

		void OnGui( EcsWorld& _world ) override;
	};
}