#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	namespace scene {
		//================================================================================================================================
		//================================================================================================================================
		class PointLight : public Component
		{
		public:
			static fan::Signal< PointLight * > onPointLightAttach;
			static fan::Signal< PointLight * > onPointLightDetach;

			bool Load(std::istream& _in)  override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;
			void OnGui() override;
			bool IsUnique()	const override { return true; }

			DECLARE_EDITOR_COMPONENT(PointLight)
			DECLARE_TYPE_INFO(PointLight);
		private:
		protected:
			void OnAttach() override;
			void OnDetach() override;
		};
	}
}