#include "render/fanFont.hpp"

#include "core/fanDebug.hpp"

namespace fan
{
    FT_Library Font::sFreetypeLib = nullptr;
    //========================================================================================================
    //========================================================================================================
    bool Font::InitFreetype()
    {
        if (FT_Init_FreeType( &sFreetypeLib ) )
        {
            Debug::Log( "ERROR::FREETYPE: Could not init FreeType Library" );
            return false;
        }
        return true;
    }

    //========================================================================================================
    //========================================================================================================
    void Font::ClearFreetype()
    {
        FT_Done_FreeType( sFreetypeLib );
    }

    //========================================================================================================
    //========================================================================================================
    bool Font::LoadFont( const std::string _path )
    {
        if( FT_New_Face( sFreetypeLib, _path.c_str(), 0, &mFace ) )
        {
            Debug::Log( "ERROR::FREETYPE: Failed to load font" );
            return false;
        }
        return true;
    }

    //========================================================================================================
    //========================================================================================================
    void Font::SetHeight( const int _pixelHeight )
    {
        if(  FT_Set_Pixel_Sizes( mFace, 0, _pixelHeight ) != 0 )
        {
            Debug::Error() << "Font::SetHeight " << _pixelHeight << " failed" << Debug::Endl();
        }
    }

    //========================================================================================================
    //========================================================================================================
    bool Font::LoadChar( const char _char )
    {
        if (FT_Load_Char( mFace, _char, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            return false;
        }
        fanAssert( mFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
        fanAssert( mFace->glyph->bitmap.num_grays == 256 );

        // now store character for later use
        Character character = {
                0,
                glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
                glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
                mFace->glyph->advance.x
        };
        mCharacters.insert(std::pair<char, Character>( _char, character) );

        Debug::Log() << "mSize" << character.mSize.x << " " << character.mSize.y << Debug::Endl();
        Debug::Log() << "mBearing" << character.mBearing.x << " " << character.mBearing.y << Debug::Endl();
        Debug::Log() << "mAdvance" << character.mAdvance << Debug::Endl();




        uint8_t * buffer = new uint8_t[character.mSize.x * character.mSize.y * 4 ];

        for( int x = 0; x < character.mSize.x; x++ )
        {
            for( int y = 0; y < character.mSize.y; y++ )
            {
                const int offset =  y * character.mSize.x + x;
                const char greyLevel = mFace->glyph->bitmap.buffer[ offset ];
                buffer[ 4 * offset + 0 ] = 255;
                buffer[ 4 * offset + 1 ] = 255;
                buffer[ 4 * offset + 2] =  255;
                buffer[ 4 * offset + 3 ] = greyLevel;
            }
        }

        mTexture = new Texture();
        mTexture->LoadFromPixels( buffer, character.mSize, 1 );

        return true;
    }
}