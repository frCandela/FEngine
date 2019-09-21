#pragma once

#include "scene/components/fanComponent.h"
#include "core/fanSignal.h"

namespace fan
{
	namespace scene
	{
		//================================================================================================================================
		//================================================================================================================================
		class Actor : public Component
		{
		public:
			static Signal< Actor * > onActorAttach;
			static Signal< Actor * > onActorDetach;

			virtual void Start() = 0;
			virtual void Update(const float _delta) = 0;

			bool IsActor() const override { return true; }
			bool IsUnique() const override { return false; }
			void OnGui() override;
			bool Load(std::istream& _in) override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;

			bool IsEnabled() const { return m_isEnabled; }
			void SetEnabled(const bool _enabled) { m_isEnabled = _enabled; }

		protected:
			void OnAttach() override;
			void OnDetach() override;

		private:
			bool m_isEnabled = true;

		};
	}
}