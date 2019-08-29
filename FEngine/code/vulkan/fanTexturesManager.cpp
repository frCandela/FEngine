#include "fanIncludes.h"

#include "vulkan/fanTexturesManager.h"

#include "vulkan/core/vkTexture.h"

namespace vk {
	//================================================================================================================================
	//================================================================================================================================
	TexturesManager::TexturesManager( Device& _device ) : 
		m_device(_device ) {
		m_textures.reserve(64);

	}

	TexturesManager::~TexturesManager(){
		for (int textureIndex = 0; textureIndex < m_textures.size() ; textureIndex++) {
			delete( m_textures[textureIndex]);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void TexturesManager::AddTexture( const std::string _path ) {

		vk::Texture * texture = new Texture( m_device );
		if( texture->LoadTexture(_path) == true ) {
			m_textures.push_back(texture);
		}
	}
}