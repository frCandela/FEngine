#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class PointLight : public Component
	{
	public:
		static Signal< PointLight * > onPointLightAttach;
		static Signal< PointLight * > onPointLightDetach;

		enum Attenuation{ CONSTANT=0, LINEAR=1, QUADRATIC=2 };

		bool Load(std::istream& _in)  override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;
		void OnGui() override;
		bool IsUnique()	const override { return true; }

		Color GetDiffuse() const { return m_diffuse; }
		void  SetDiffuse(const Color _diffuse);
		Color GetSpecular() const { return m_specular; }
		void  SetSpecular(const Color m_specular);
		float GetAttenuation( const Attenuation _attenuation ) const { return m_attenuation[_attenuation]; }
		void  SetAttenuation( const Attenuation _attenuation, const float _value );

		DECLARE_EDITOR_COMPONENT(PointLight)
		DECLARE_TYPE_INFO(PointLight);
	protected:
		PointLight();
		void OnAttach() override;
		void OnDetach() override;

	private:
		Color m_diffuse;
		Color m_specular;
		float m_attenuation[3];

		float GetLightRange() const;
	};
}