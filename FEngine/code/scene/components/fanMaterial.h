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

		// Getters
		void			SetTexture( Texture * const _texture);
		Texture *		GetTexture();
		const Texture *	GetTexture() const;
		void			SetShininess(const uint32_t _shininess );
		const uint32_t	GetShininess() const;
		Color			GetColor() const;
		void			SetColor( const Color _color );



		void OnGui() override;
		bool IsUnique()	const override { return true; }

		DECLARE_EDITOR_COMPONENT(Material)
		DECLARE_TYPE_INFO(Material);
	protected:
		void OnAttach() override;
		void OnDetach() override;
		ecsMaterial* GetEcsMaterial() const;
	private:


		// Editor
		std::fs::path m_pathBuffer;
	};
}