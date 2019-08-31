#include "fanIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanModel.h"
#include "renderer/core/vkTexture.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/vkRenderer.h"

namespace scene {
	REGISTER_TYPE_INFO(Material)
		
	util::Signal< Material * > Material::onRegisterMaterial;
	util::Signal< Material * > Material::onUnregisterMaterial;

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
			vk::RessourceManager * texturesManager = vk::Renderer::GetRenderer().GetRessourceManager();
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

		_out << "\t\t" << (m_texture != nullptr ? m_texture->GetPath() : "void" ) << std::endl;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexture(vk::Texture * _texture) {
		m_texture = _texture;
		SetModified( true );
	}
}