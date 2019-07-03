#pragma once

#include "editor/fanWindow.h"

namespace scene {
	class Transform;
	class Component;
}

namespace editor {
	class InspectorWindow : public editor::Window{
	public:
		InspectorWindow();

		void Draw() override;

	private:
		void DrawComponent(scene::Component & _component);
		void DrawTransform(scene::Transform & _transform);
	};
}
