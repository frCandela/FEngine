#pragma once

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	struct Texture;
	struct Device;

	//========================================================================================================
	//========================================================================================================
	class TextureManager
	{
	public:
        TextureManager();

        Texture*    Get( const std::string& _path ) const;
        Texture*    Load( const std::string& _path );
        Texture*    GetOrLoad( const std::string& _path );
        void        Add( Texture* _texture, const std::string& _name );
        void        Remove( const std::string& _path );
	    void        Clear( Device& _device );
        bool        Empty() const { return mTextures.empty(); }
        void	    ResolvePtr( ResourcePtr< Texture >& _resourcePtr );

        bool        Create( Device& _device );
        void        Destroy( Device& _device );

        int         DestroyListSize() const  { return (int)mDestroyList.size(); }
        const std::vector< Texture * >& GetTextures() const { return mTextures; }

	private:
        std::vector< Texture * >    mTextures;
        std::vector< Texture * >    mDestroyList;
	};
}