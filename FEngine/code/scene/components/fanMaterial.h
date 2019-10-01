#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	class Texture;
	class Model;

	//================================================================================================================================
	//================================================================================================================================
	class Material : public Component
	{
	public:
		static Signal< Material *>					onMaterialAttach;
		static Signal< Material * >					onMaterialDetach;
		static Signal< Material *, std::string  >	onMaterialSetPath;

		bool Load(std::istream& _in)	override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		void			SetTexture( Texture * const _texture);
		Texture *		GetTexture() { return m_texture; }
		const Texture *	GetTexture() const { return m_texture; }
		void			SetShininess(const uint32_t _shininess );
		const uint32_t	GetShininess() const { return m_shininess;  }
		Color			GetColor() const	 {	return m_color;		}
		void			SetColor( const Color _color );

		void OnGui() override;
		bool IsUnique()	const override { return true; }

		DECLARE_EDITOR_COMPONENT(Material)
		DECLARE_TYPE_INFO(Material);
	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		Texture * m_texture = nullptr;
		uint32_t  m_shininess = 1;
		Color m_color = Color::White;

		// Editor
		std::fs::path m_pathBuffer;
	};
}