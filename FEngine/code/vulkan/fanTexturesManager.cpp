#include "fanIncludes.h"

#include "vulkan/fanTexturesManager.h"

#include "vulkan/core/vkTexture.h"
#include "core/fanSignal.h"

namespace vk {
	const char * TexturesManager::s_defaultTexture = "content/_default/texture.png";
	util::Signal<> TexturesManager::onTextureLoaded;

	//================================================================================================================================
	//================================================================================================================================
	TexturesManager::TexturesManager( Device& _device ) : 
		m_device(_device ) {
		m_textures.reserve(64);

		LoadTexture(s_defaultTexture);
	}

	//================================================================================================================================
	//================================================================================================================================
	TexturesManager::~TexturesManager(){
		for (int textureIndex = 0; textureIndex < m_textures.size() ; textureIndex++) {
			delete( m_textures[textureIndex]);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	vk::Texture * TexturesManager::LoadTexture( const std::string _path ) {
		// Dont add the texture if it already exists
		if (FindTexture(_path) != nullptr) {
			fan::Debug::Get() << fan::Debug::Severity::warning << "Texture already added: " << _path << std::endl;
			return nullptr;
		}

		// Add
		vk::Texture * texture = new Texture( m_device );
		if( texture->LoadTexture(_path) == true ) {
			texture->SetRenderID(static_cast<int>(m_textures.size()));
			m_textures.push_back(texture);
			onTextureLoaded.Emmit();
		}

		return texture;
	}

	//================================================================================================================================
	//================================================================================================================================
	uint32_t TexturesManager::FindTextureIndex(const vk::Texture * _texture) {
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			if (m_textures[textureIndex] == _texture) {
				return textureIndex;
			}
		}
		return 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	vk::Texture * TexturesManager::FindTexture( const std::string _path ) {
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			const vk::Texture * texture = m_textures[textureIndex];
			if (texture->GetPath() == _path) {				
				return m_textures[textureIndex];
			}
		}
		return nullptr;
	}
}