#include "fanIncludes.h"

#include "vulkan/fanTexturesManager.h"

#include "vulkan/core/vkTexture.h"

namespace vk {
	const char * TexturesManager::s_defaultTexture = "content/_default/texture.png";

	//================================================================================================================================
	//================================================================================================================================
	TexturesManager::TexturesManager( Device& _device ) : 
		m_device(_device ) {
		m_textures.reserve(64);

		AddTexture(s_defaultTexture);
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
	void TexturesManager::AddTexture( const std::string _path ) {
		// Dont add the texture if it already exists
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			const vk::Texture * texture = m_textures[textureIndex];
			if (texture->GetPath() == _path) {
				fan::Debug::Get() << fan::Debug::Severity::warning << "Texture already added: " << _path << std::endl;
				return;
			}
		}

		// Add
		vk::Texture * texture = new Texture( m_device );
		if( texture->LoadTexture(_path) == true ) {
			m_textures.push_back(texture);
		}
	}
}