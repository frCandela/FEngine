#include "render/resources/fanFont.hpp"
#include "core/fanAssert.hpp"
#include "render/resources/fanFontManager.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    FontManager::FontManager()
    {
        if (FT_Init_FreeType( &mFreetypeLib ) )
        {
            Debug::Error( "FontManager: Could not init FreeType Library" );
            return;
        }
    }

    //========================================================================================================
    //========================================================================================================
    FontManager::~FontManager()
    {
        FT_Done_FreeType( mFreetypeLib );
    }

    //========================================================================================================
    //========================================================================================================
    Font* FontManager::Load( const std::string& _path )
    {
        fanAssert( ! _path.empty() );
        Font* font = Find( _path );
        if( font != nullptr ){ return font;}

        Debug::Log() << "Loading font " << _path << Debug::Endl();
        font = new Font( mFreetypeLib, _path );
        if( ! font->IsValid() )
        {
            delete font;
            return nullptr;
        }
        mFonts.push_back( font );
        return font;
    }

    //========================================================================================================
    //========================================================================================================
    void FontManager::Remove( const std::string& _path )
    {
        for( int i = 0; i < mFonts.size(); i++ )
        {
            Font* font = mFonts[i];
            if( font->GetPath() == _path )
            {
                mFonts[i] = * mFonts.rbegin();
                mFonts.pop_back();
                delete font;
                return;
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    Font* FontManager::Find( const std::string& _path ) const
    {
        for( Font* font : mFonts )
        {
            if( font->GetPath() == _path )
            {
                return font;
            }
        }
        return nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void FontManager::Clear()
    {
        for ( Font* font : mFonts )
        {
            delete font;
        }
        mFonts.clear();
    }
}