#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {
	namespace scene { class Camera; }

	namespace game {
		//================================================================================================================================
		//================================================================================================================================	
		class GameManager : public scene::Actor {
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
			scene::Camera * m_camera;
		};
	}
}