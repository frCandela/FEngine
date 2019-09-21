#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
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
		class InspectorWindow : public editor::Window {
		public:
			InspectorWindow();

		protected:
			void OnGui() override;

		private:
			void NewComponentPopup();
		};
	}
}
