#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {
	namespace scene { 
		class Camera; 
		class Transform;
	}

	namespace game {
		//================================================================================================================================
		//================================================================================================================================	
		class CameraController : public scene::Actor {
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
			scene::Camera *		m_camera;
			scene::Transform *	m_transform;
			scene::Transform *	m_target;

			float m_heightFromTarget;
		};
	}
}