#pragma once

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Texture;
	class Device;

	//================================================================================================================================
	// Owns all the textures of the engine
	// load, find, resolve pointers, delete
	//================================================================================================================================
	class TextureManager
	{
	public:
		void Init( Device& _device );
		void Clear();

		bool IsCleared() { return m_device == nullptr; } //@hack ressources gpu buffers deletion problem

		void	 ResolvePtr( ResourcePtr< Texture >& _resourcePtr );
		Device&  GetDevice() const { assert( m_device != nullptr ); return *m_device; }
		Texture* GetTexture( const std::string& _path );
		Texture* FindTexture( const std::string& _path );

		bool IsModified() const { return m_modified; }
		void SetUnmodified()    { m_modified = false; }
		const std::vector< Texture* >& GetList() const { return m_textureList; }

	private:
		Device* m_device = nullptr;
		std::vector< Texture* > m_textureList;
		bool					m_modified = false;		
	};
}