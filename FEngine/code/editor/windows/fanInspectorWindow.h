#pragma once

#include "editor/windows/fanEditorWindow.h"

namespace fan
{
	class Transform;
	class Component;
	class Camera;
	class Model;
	class FPSCamera;
	class Material;

	//================================================================================================================================
	//================================================================================================================================
	class InspectorWindow : public EditorWindow {
	public:
		InspectorWindow();

	protected:
		void OnGui() override;

	private:
		void NewComponentPopup();
	};
}