#pragma once

#include "editor/fanWindow.h"

namespace scene {
	class Transform;
	class Component;
	class Camera;
}

namespace editor {
	class InspectorWindow : public editor::Window{
	public:
		InspectorWindow();

		void Draw() override;

	private:
		void DrawComponent(scene::Component & _component);
		void DrawTransform(scene::Transform & _transform);
		void DrawCamera(scene::Camera & _camera);
		
	};
}
