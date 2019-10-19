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

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::POINT_LIGHT; }

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
		DECLARE_TYPE_INFO(PointLight, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json )  override;
		bool Save( Json & _json ) const override;
		ecsPointLight*  GetEcsPointLight() const;

	private:
		float GetLightRange() const;
	};
}