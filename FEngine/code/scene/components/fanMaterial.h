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
		static Signal< Material * > onMaterialAttach;
		static Signal< Material * > onMaterialDetach;

		bool Load(std::istream& _in)	override;
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		void				SetTexture(Texture * _texture);
		Texture *		GetTexture() { return m_texture; }
		const Texture *	GetTexture() const { return m_texture; }

		void OnGui() override;
		bool IsUnique()	const override { return true; }

		DECLARE_EDITOR_COMPONENT(Material)
		DECLARE_TYPE_INFO(Material);
	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		Texture * m_texture = nullptr;

		// Editor
		std::fs::path m_pathBuffer;
	};
}