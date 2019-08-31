#pragma once

namespace vk {
	class Texture;
	class Device;

	//================================================================================================================================
	// Loads & references all the textures of the engine
	//================================================================================================================================
	class RessourceManager {
	public:
		static util::Signal<> onTextureLoaded;

		RessourceManager( Device& _device );
		~RessourceManager();

		vk::Texture * LoadTexture( const std::string _path );

		uint32_t								FindTextureIndex(const vk::Texture * _texture );
		vk::Texture *							FindTexture( const std::string _path );
		const std::vector< vk::Texture * > &	GetTextures() const	{ return m_textures; }
		std::vector< vk::Texture * > &			GetTextures()		{ return m_textures; }
		
		static const char * s_defaultTexture;

	private:
		vk::Device & m_device;
		std::vector< vk::Texture * > m_textures;
	};
}