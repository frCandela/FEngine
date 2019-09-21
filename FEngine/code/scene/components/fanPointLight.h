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
			static Signal< PointLight * > onPointLightAttach;
			static Signal< PointLight * > onPointLightDetach;

			bool Load(std::istream& _in)  override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;
			void OnGui() override;
			bool IsUnique()	const override { return true; }

			Color GetColor() const { return m_color; }
			void  SetColor( const Color _color );

			DECLARE_EDITOR_COMPONENT(PointLight)
			DECLARE_TYPE_INFO(PointLight);
		protected:
			void OnAttach() override;
			void OnDetach() override;

		private:
			Color m_color;
		};
	}
}