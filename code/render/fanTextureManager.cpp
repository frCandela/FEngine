#include "render/fanTextureManager.hpp"

#include <filesystem>
#include "core/fanDebug.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void TextureManager::Init( Device& _device )
	{
		m_device = &_device;
		ResourcePtr< Texture >::s_onResolve.Connect( &TextureManager::ResolvePtr, this );
		GetTexture( RenderGlobal::s_defaultTexture );
		GetTexture( RenderGlobal::s_whiteTexture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TextureManager::Clear()
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle texture manager" );
		while ( !m_textureList.empty() ) 
		{ 
			Texture* texture = m_textureList[m_textureList.size() - 1];
			texture->Destroy( *m_device );
			delete texture;
			m_textureList.pop_back();
		}
		m_device = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void TextureManager::ResolvePtr( ResourcePtr< Texture >& _resourcePtr )
	{
		assert( ! _resourcePtr.IsValid() );

		TexturePtr& texturePtr = static_cast< TexturePtr& >( _resourcePtr );
		Texture * texture = GetTexture( texturePtr.GetPath() );

		if ( texture )
		{
			texturePtr = texture;
		}
	}

	//================================================================================================================================
	// returns a texture at a specific path
	// First try to find it, else loads it from a path and registers it
	//================================================================================================================================
	Texture* TextureManager::GetTexture( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();

		Texture* texture = FindTexture( cleanPath );
		if ( texture != nullptr )
		{
			return texture;
		}
		else
		{
			// Load
			texture = new Texture();			
			if ( texture->CreateFromFile( Texture::s_resourceManager.GetDevice(), cleanPath ) )
			{	
				texture->renderID =  static_cast< int >( m_textureList.size() );
				m_textureList.push_back( texture );
				m_modified = true;
				return texture;
			}
			delete texture;
			return nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture* TextureManager::FindTexture( const std::string& _path )
	{
		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
		for (int textureIndex = 0; textureIndex < m_textureList.size() ; textureIndex++)
		{
			Texture& texture = *m_textureList[ textureIndex ];
			if ( texture.path == cleanPath )
			{
				return &texture;
			}
		}		
		return nullptr;
	}
}