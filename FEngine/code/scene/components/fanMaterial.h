#pragma once

#include "scene/components/fanComponent.h"

namespace vk{ class Texture; }

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	class Material : public Component
	{
	public:

		bool			IsUnique()		const override { return true; }

		void			SetTexture( const vk::Texture * m_texture);
		void			SetTexture( const std::string _path);
		vk::Texture *	GetTexture() { return m_texture; }

		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		DECLARE_TYPE_INFO(Material);
	private:
		void Initialize() override;

		vk::Texture * m_texture = nullptr;
	};
}