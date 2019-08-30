#pragma once

namespace vk {
	class Texture;
	class Device;

	//================================================================================================================================
	// Loads & references all the textures of the engine
	//================================================================================================================================
	class TexturesManager {
	public:
		TexturesManager( Device& _device );
		~TexturesManager();

		void AddTexture( const std::string _path );

		const std::vector< vk::Texture * > &	GetTextures() const	{ return m_textures; }
		std::vector< vk::Texture * > &			GetTextures()		{ return m_textures; }
		
		static const char * s_defaultTexture;

	private:
		vk::Device & m_device;
		std::vector< vk::Texture * > m_textures;
	};
}