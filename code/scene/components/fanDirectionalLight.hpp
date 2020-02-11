#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class DirectionalLight : public Component
	{
	public:
		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::DIR_LIGHT16; }

		Color GetAmbiant() const;
		Color GetDiffuse() const;
		Color GetSpecular() const;
		void  SetAmbiant( const Color _ambiant );
		void  SetDiffuse( const Color _diffuse );
		void  SetSpecular( const Color m_specular );

		DECLARE_TYPE_INFO( DirectionalLight, Component );
	protected:
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;
		void OnAttach() override;
		void OnDetach() override;

	private:
		ecsDirLight* const m_dirLight = nullptr;
	};
}