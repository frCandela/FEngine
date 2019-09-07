#include "fanGlobalIncludes.h"

#include "scene/fanEntity.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanModel.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	namespace scene {
		REGISTER_TYPE_INFO(Material)

			fan::Signal< Material * > Material::onRegisterMaterial;
		fan::Signal< Material * > Material::onUnregisterMaterial;

		//================================================================================================================================
		//================================================================================================================================
		void Material::Initialize() {
			onRegisterMaterial.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Material::Delete() {
			onUnregisterMaterial.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Material::Load(std::istream& _in) {
			std::string path;
			_in >> path;
			if (path != std::string("void")) {
				// TODO find a cleaner way to set the texture
				vk::RessourceManager * texturesManager = Renderer::Get().GetRessourceManager();
				vk::Texture * texture = texturesManager->FindTexture(path);
				if (texture == nullptr) {
					texture = texturesManager->LoadTexture(path);
				}
				SetTexture(texture);
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Material::Save(std::ostream& _out) {

			_out << "\t\t" << (m_texture != nullptr ? m_texture->GetPath() : "void") << std::endl;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Material::SetTexture(vk::Texture * _texture) {
			m_texture = _texture;
			SetModified(true);
		}
	}
}