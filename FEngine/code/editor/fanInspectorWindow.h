#pragma once

#include "editor/fanWindow.h"

namespace scene {
	class Transform;
	class Component;
	class Camera;
	class Mesh;
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
		void DrawMesh(scene::Mesh & _mesh);

		std::experimental::filesystem::path m_cachePathMesh;
		std::set < std::string > m_meshExtensionFilter;
	};
}
