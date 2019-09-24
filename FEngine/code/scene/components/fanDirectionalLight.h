#pragma once

#include "scene/components/fanComponent.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class DirectionalLight : public Component {
	public:
		static Signal< DirectionalLight * > onDirectionalLightAttach;
		static Signal< DirectionalLight * > onDirectionalLightDetach;

		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		bool Load( std::istream& _in )  override;
		bool Save( std::ostream& _out, const int _indentLevel ) const override;
		void OnGui() override;
		bool IsUnique()	const override { return true; }

		Color GetAmbiant() const  { return m_ambiant; }
		Color GetDiffuse() const  { return m_diffuse; }
		Color GetSpecular() const { return m_specular; }
		void  SetAmbiant(  const Color _ambiant );
		void  SetDiffuse(  const Color _diffuse );
		void  SetSpecular( const Color m_specular );

		DECLARE_EDITOR_COMPONENT( DirectionalLight )
		DECLARE_TYPE_INFO( DirectionalLight );
	protected:
		DirectionalLight();
		void OnAttach() override;
		void OnDetach() override;

	private:
		Color m_ambiant;
		Color m_diffuse;
		Color m_specular;
	};
}