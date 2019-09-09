#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {
	namespace game {
		class Planet : public scene::Actor {
		public:
			bool IsUnique() const override { return true; }

			void Start() override;
			void Update(const float _delta) override;
			bool Load(std::istream& /*_in*/) override { return true; }
			bool Save(std::ostream& /*_out*/, const int /*_indentLevel*/) const override { return true; }

			DECLARE_TYPE_INFO(Planet);
		protected:
			void Initialize() override;
			void Delete() override {};

			static bool m_registered;

			float m_dir = -1.f;
		};
	}
}