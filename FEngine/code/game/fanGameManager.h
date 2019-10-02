#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {

	class Camera;
	class FPSCamera;
	class SpaceShip;

	//================================================================================================================================
	//================================================================================================================================	
	class GameManager : public Actor {
	public:

		void Start() override;
		void Update(const float _delta) override;
		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		bool IsUnique() const override { return true; }
		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(GameManager)
		DECLARE_TYPE_INFO(GameManager);
	protected:
	private:
		Camera * m_camera;
		SpaceShip * m_spaceShip;
		FPSCamera * m_editorCameraController;

		void SwitchCameras();
	};
}