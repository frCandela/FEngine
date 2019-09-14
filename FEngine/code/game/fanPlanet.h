#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {
	namespace game {
		//================================================================================================================================
		//================================================================================================================================
		class Planet : public scene::Actor {
		public:

			void Start() override;
			void Update(const float _delta) override;
			bool Load(std::istream& _in) override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;

			void OnGui() override;
			bool IsUnique() const override { return true; }

			DECLARE_EDITOR_COMPONENT(Planet)
			DECLARE_TYPE_INFO(Planet);
		protected:
		private:

			float m_speed  = 1.f;
			float m_radius = 1.f;
		};
	}
}