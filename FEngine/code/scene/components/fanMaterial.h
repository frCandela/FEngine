#pragma once

#include "scene/components/fanComponent.h"

namespace vk {
	class Texture;
}

namespace scene {
	class Model;

	//================================================================================================================================
	//================================================================================================================================
	class Material : public Component
	{
	public:
		static util::Signal< Material * > onMaterialCreated;
		static util::Signal< Material * > onMaterialDeleted;

		bool IsUnique()		const override { return true; }
		void Load(std::istream& _in)	override;
		void Save(std::ostream& _out)	override;


		void SetTexture(vk::Texture * _texture);
		vk::Texture *	GetTexture() { return m_texture; }
		const vk::Texture *	GetTexture() const { return m_texture; }

		DECLARE_TYPE_INFO(Material);
	private:
		void Initialize() override;
		void Delete() override;

		vk::Texture * m_texture = nullptr;
	};
}