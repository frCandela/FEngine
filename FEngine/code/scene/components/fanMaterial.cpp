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
		void Material::OnDetach() {
			Component::OnDetach();
			onUnregisterMaterial.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Material::OnAttach() {
			Component::OnAttach();
			onRegisterMaterial.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Material::Load(std::istream& _in) {
			std::string path;
			if (!ReadSegmentHeader(_in, "material:")) { return false; }
			if (!ReadString(_in, path)) { return false; }

			if (path != std::string("void")) {
				// TODO find a cleaner way to set the texture
				vk::RessourceManager * texturesManager = Renderer::Get().GetRessourceManager();
				vk::Texture * texture = texturesManager->FindTexture(path);
				if (texture == nullptr) {
					texture = texturesManager->LoadTexture(path);
				}
				SetTexture(texture);
			}
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Material::Save(std::ostream& _out, const int _indentLevel) const{
			const std::string indentation = GetIndentation(_indentLevel);
			_out << indentation <<  "material: " << (m_texture != nullptr ? m_texture->GetPath() : "void") << std::endl;
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Material::SetTexture(vk::Texture * _texture) {
			m_texture = _texture;
			SetModified(true);			
		}
	}
}