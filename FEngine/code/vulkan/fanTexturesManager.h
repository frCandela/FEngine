#pragma once

namespace vk {
	class Texture;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class TexturesManager {
	public:
		TexturesManager( Device& _device );
		~TexturesManager();

		void AddTexture( const std::string _path );		

		const std::vector< vk::Texture * > & GetTextures() const { return m_textures; }
		std::vector< vk::Texture * > & GetTextures() { return m_textures; }
	
	private:
		vk::Device & m_device;
		std::vector< vk::Texture * > m_textures;
	};
}