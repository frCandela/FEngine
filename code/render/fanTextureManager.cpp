#include "render/fanTextureManager.hpp"

#include "render/core/fanTexture.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void TextureManager::Init( Device& _device )
	{
		m_device = &_device;
		ResourcePtr< Texture >::s_onResolve.Connect( &TextureManager::ResolvePtr, this );
		LoadTexture( RenderGlobal::s_defaultTexture );
		LoadTexture( RenderGlobal::s_whiteTexture );
	}

	//================================================================================================================================
	//================================================================================================================================
	void TextureManager::Clear()
	{
		while ( !m_textureList.empty() ) 
		{ 
			delete m_textureList[ m_textureList.size() - 1]; 
			m_textureList.pop_back();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void TextureManager::ResolvePtr( ResourcePtr< Texture >& _resourcePtr )
	{
		assert( ! _resourcePtr.IsValid() );

		TexturePtr& texturePtr = static_cast< TexturePtr& >( _resourcePtr );
		Texture * texture = LoadTexture( texturePtr.GetPath() );

		if ( texture )
		{
			texturePtr = texture;
		}
	}

	//================================================================================================================================
	// Load a mesh from a path, loads it and registers it
	//================================================================================================================================
	Texture* TextureManager::LoadTexture( const std::string& _path )
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
			if ( texture->LoadFromFile( cleanPath ) )
			{	
				texture->SetRenderID( static_cast< int >( m_textureList.size() ) );
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
			if ( texture.GetPath() == cleanPath )
			{
				return &texture;
			}
		}		
		return nullptr;
	}
}