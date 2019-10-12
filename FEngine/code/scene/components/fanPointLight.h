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

		// Getters
		Color		GetAmbiant		(						) const;
		void		SetAmbiant		( const Color _ambiant	);
		Color		GetDiffuse		(						) const;
		void		SetDiffuse		( const Color _diffuse	);
		Color		GetSpecular		(						) const;
		void		SetSpecular		( const Color m_specular);
		glm::vec3	GetAttenuation	(						) const;
		float		GetAttenuation	( const Attenuation _attenuation					 ) const;
		void		SetAttenuation	( const Attenuation _attenuation, const float _value );

		DECLARE_EDITOR_COMPONENT(PointLight)
		DECLARE_TYPE_INFO(PointLight);
	protected:
		void OnAttach() override;
		void OnDetach() override;
		ecsPointLight*  GetEcsPointLight() const;

	private:
		float GetLightRange() const;
	};
}