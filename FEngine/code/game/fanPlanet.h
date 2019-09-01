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
			void Load(std::istream& /*_in*/) override {}
			void Save(std::ostream& /*_out*/) override {}

			DECLARE_TYPE_INFO(Planet);
		protected:
			void Initialize() override;
			void Delete() override {};

			static bool m_registered;

			float m_dir = -1.f;
		};
	}
}