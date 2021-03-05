#include "render/resources/fanTextureManager.hpp"
#include "core/fanFileSystem.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    TextureManager::TextureManager()
    {
        Load( RenderGlobal::sDefaultTexture );
    }

    //========================================================================================================
    //========================================================================================================
    Texture* TextureManager::Get( const std::string& _path ) const
    {
        for( Texture* texture : mTextures )
        {
            if( texture->mPath == _path )
            {
                return texture;
            }
        }
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    Texture* TextureManager::Load( const std::string& _path )
    {
        Texture* texture = Get( _path );
        if( texture != nullptr ) { return texture; }

        // Load
        texture = new Texture();
        if( texture->LoadFromFile( _path ) )
        {
            texture->mIndex = (int)mTextures.size();
            mTextures.push_back( texture );
            return texture;
        }
        delete texture;
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    Texture* TextureManager::GetOrLoad( const std::string& _path )
    {
        Texture * texture = Get( _path );
        if( texture == nullptr )
        {
            texture = Load( _path );
        }
        return texture;
    }

    //========================================================================================================
    // Every texture added using this method will be deleted by the TextureManager
    //========================================================================================================
    void TextureManager::Add( Texture* _texture, const std::string& _name )
    {
       fanAssert( _texture != nullptr );
        _texture->mIndex = (int)mTextures.size();
        _texture->mPath = _name;
        mTextures.push_back( _texture );
    }

    //========================================================================================================
    //========================================================================================================
    void TextureManager::Remove( const std::string& _path )
    {
        for( int textureIndex = 0; textureIndex < mTextures.size(); textureIndex++ )
        {
            if( mTextures[textureIndex]->mPath == _path )
            {
                ( *mTextures.rbegin() )->mIndex = textureIndex;
                mDestroyList.push_back( mTextures[ textureIndex ]);
                mTextures[textureIndex] = mTextures[mTextures.size() - 1];
                mTextures.pop_back();
                return;
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    void TextureManager::Clear( Device& _device )
    {
        for ( Texture* texture : mTextures )
        {
            texture->Destroy( _device );
            delete texture;
        }
        mTextures.clear();
        DestroyRemovedTextures( _device );
    }

    //========================================================================================================
    //========================================================================================================
    void TextureManager::ResolvePtr( ResourcePtr<Texture >& _resourcePtr )
    {
       fanAssert( ! _resourcePtr.IsValid() );

        TexturePtr& texturePtr = static_cast< TexturePtr& >( _resourcePtr );
        Texture    * texture    = GetOrLoad( texturePtr.GetPath() );
        if ( texture != nullptr )
        {
            texturePtr = texture;
        }
    }

    //========================================================================================================
    //========================================================================================================
    bool TextureManager::CreateNewTextures( Device& _device )
    {
        bool textureCreated = false;
        for( Texture * texture : mTextures )
        {
            if( texture->mBuffersOutdated )
            {
                texture->Create( _device );
                textureCreated = true;
            }
        }
        return textureCreated;
    }

    //========================================================================================================
    //========================================================================================================
    void TextureManager::DestroyRemovedTextures( Device& _device )
    {
        for( Texture* texture : mDestroyList )
        {
            texture->Destroy( _device );
            delete texture;
        }
        mDestroyList.clear();
    }
}