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
		bool Load(std::istream& _in) override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		bool IsUnique() const override { return true; }
		void OnGui() override;
		void OnDetach() override;

		DECLARE_EDITOR_COMPONENT(CameraController)
		DECLARE_TYPE_INFO(CameraController);
	protected:
	private:
		Camera *		m_camera;
		Transform *	m_transform;
		Transform *	m_target;

		float m_heightFromTarget;
	};
}