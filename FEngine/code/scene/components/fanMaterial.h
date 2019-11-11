#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	class Texture;
	class MeshRenderer;

	//================================================================================================================================
	//================================================================================================================================
	class Material : public Component
	{
	public:
		static Signal< Material *>	onMaterialAttach;
		static Signal< Material * >	onMaterialDetach;

		// Getters
		void			SetTexture( Texture * const _texture);
		Texture *		GetTexture();
		const Texture *	GetTexture() const;
		void			SetShininess(const uint32_t _shininess );
		const uint32_t	GetShininess() const;
		Color			GetColor() const;
		void			SetColor( const Color _color );
		void			SetTexturePath( const std::string& _path );

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::MATERIAL; }

		DECLARE_EDITOR_COMPONENT(Material)
		DECLARE_TYPE_INFO(Material, Component );
	protected:
		bool Load( Json & _json )	override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;
		void OnDetach() override;

	private:
		ecsMaterial * const m_material = nullptr;
	};
}