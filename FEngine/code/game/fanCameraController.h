#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {

	class Camera;
	class Transform;

	//================================================================================================================================
	//================================================================================================================================	
	class CameraController : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;

		void OnGui() override;
		void OnDetach() override;

		DECLARE_EDITOR_COMPONENT(CameraController)
		DECLARE_TYPE_INFO(CameraController);
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		Camera *		m_camera;
		Transform *	m_transform;
		Transform *	m_target;

		float m_heightFromTarget;
	};
}