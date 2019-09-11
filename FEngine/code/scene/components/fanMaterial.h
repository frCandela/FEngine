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
			static fan::Signal< Material * > onMaterialAttach;
			static fan::Signal< Material * > onMaterialDetach;

			bool IsUnique()		const override { return true; }
			bool Load(std::istream& _in)	override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;


			void SetTexture(vk::Texture * _texture);
			vk::Texture *	GetTexture() { return m_texture; }
			const vk::Texture *	GetTexture() const { return m_texture; }

			DECLARE_TYPE_INFO(Material);
		protected:
			void OnAttach() override;
			void OnDetach() override;

		private:
			vk::Texture * m_texture = nullptr;


		};
	}
}