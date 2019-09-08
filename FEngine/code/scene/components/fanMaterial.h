#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
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
			static fan::Signal< Material * > onRegisterMaterial;
			static fan::Signal< Material * > onUnregisterMaterial;

			bool IsUnique()		const override { return true; }
			void Load(std::istream& _in)	override;
			void Save(std::ostream& _out, const int _indentLevel)	override;


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
}