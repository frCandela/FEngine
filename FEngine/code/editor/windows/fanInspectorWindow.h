#pragma once

#include "editor/windows/fanWindow.h"

namespace scene {
	class Transform;
	class Component;
	class Camera;
	class Model;
	class FPSCamera;
	class Material;
}

namespace editor {
	//================================================================================================================================
	//================================================================================================================================
	class InspectorWindow : public editor::Window{
	public:
		InspectorWindow();

		void Draw() override;

	private:		
		void NewComponentPopup	();
		void DrawComponent		( scene::Component & _component	);
		void DrawTransform		( scene::Transform & _transform	);
		void DrawCamera			( scene::Camera & _camera		);
		void DrawModel			( scene::Model & _model			);
		void DrawFPSCamera		( scene::FPSCamera & _fpsCamera	);
		void DrawMaterial		( scene::Material & _material	);

		std::experimental::filesystem::path m_pathBuffer;
	};
}
