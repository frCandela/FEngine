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

		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(GameManager)
		DECLARE_TYPE_INFO(GameManager);
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		Camera * m_camera;
		SpaceShip * m_spaceShip;
		FPSCamera * m_editorCameraController;

		void SwitchCameras();
	};
}