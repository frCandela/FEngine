#include "render/fanFont.hpp"
#include "SFML/System/Utf.hpp"
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
            Debug::Error( "Freetype: Failed to load font" );
            return false;
        }

        if( FT_Select_Charmap( mFace, FT_ENCODING_UNICODE ) )
        {
            Debug::Error( "Freetype: font has no unicode charmap" );
            return false;
        }

        return true;
    }

    //========================================================================================================
    //========================================================================================================
    bool Font::SetHeight( const int _pixelHeight )
    {
        if(  FT_Set_Pixel_Sizes( mFace, 0, _pixelHeight ) != 0 )
        {
            Debug::Error() << "Font::SetHeight " << _pixelHeight << " failed" << Debug::Endl();
            return false;
        }
        mGlyphSize = _pixelHeight;
        return true;
    }


    //========================================================================================================
    //========================================================================================================
    void Font::ToUTF8( const std::string& _str, std::vector<uint32_t >& _outUnicode )
    {
        _outUnicode.clear();
        std::string::const_iterator it = _str.begin();
        while ( it != _str.end())
        {
            uint32_t codepoint;
            it = sf::Utf8::decode(it, _str.end(), codepoint );
            _outUnicode.push_back(codepoint);
        }
    }

    //========================================================================================================
    //========================================================================================================
    const Font::Glyph& Font::GetGlyph( const uint32_t _codepoint ) const
    {
        auto it = mGlyphs.find( _codepoint );
        if( it == mGlyphs.end() )
        {
            std::vector<uint32_t> defaultUnicode;
            Font::ToUTF8( "?", defaultUnicode );
            it = mGlyphs.find( defaultUnicode[0] );
        }
        fanAssert( it != mGlyphs.end() );
        return it->second;
    }

    //========================================================================================================
    //========================================================================================================
    int FindAtlasSize( const size_t _numElements )
    {
        size_t size = 1;
        while( size * size < _numElements )
        {
            size *= 2;
        }
        return (int)size;
    }

    //========================================================================================================
    //========================================================================================================
    Texture* Font::GenerateAtlas()
    {
        fanAssert( mGlyphSize != 0 );
        fanAssert( mFace->charmap != nullptr );

        std::vector< FT_ULong > unicodeCharacters;
        FT_UInt index;
        FT_ULong character = FT_Get_First_Char( mFace, &index);
        while( true )
        {
            unicodeCharacters.push_back( character );
            character = FT_Get_Next_Char(mFace, character, &index);
            if (!index) break;
        }

        mAtlasSize = FindAtlasSize( unicodeCharacters.size() );
        const size_t bufferPixelSize = mGlyphSize * mAtlasSize;
        uint8_t * buffer = new uint8_t[ bufferPixelSize * bufferPixelSize * 4 ];
        Texture * texture = new Texture();

        glm::ivec2 glyphCoord(0,0);
        for ( int  i = 0; i < unicodeCharacters.size(); i++)
        {
            fanAssert( i < 256 );
            const unsigned long charcode = unicodeCharacters[i];
            const unsigned glyphIndex  =  FT_Get_Char_Index( mFace, charcode );
            if( FT_Load_Glyph( mFace, glyphIndex,  FT_LOAD_DEFAULT  ) != 0 )
            {
                Debug::Log("error");
                continue;
            }

            if( FT_Render_Glyph( mFace->glyph, FT_RENDER_MODE_NORMAL  ) != 0 )
            {
                Debug::Log("error");
                continue;
            }

            const glm::ivec2 glyphSize   = glm::ivec2( mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows);
            if( glyphSize.x > mGlyphSize || glyphSize.y > mGlyphSize ){ continue; }

            const glm::ivec2 glyphOrigin = mGlyphSize * glyphCoord;
            const glm::ivec2 glyphBearing = glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top);
            Glyph glyph = {
                    glyphOrigin,
                    glyphSize,
                    glyphBearing,
                    mFace->glyph->advance.x
            };
            mGlyphs[charcode] = glyph;

            for( int x = 0; x < glyph.mSize.x; x++ )
            {
                for( int y = 0; y < glyph.mSize.y; y++ )
                {
                    const size_t glyphLocalOffset = y * glyph.mSize.x + x;
                    const char greyLevel = mFace->glyph->bitmap.buffer[ glyphLocalOffset ];

                    glm::ivec2   pixel = glyphOrigin + glm::ivec2( x, y );
                    const size_t offset = pixel.y * bufferPixelSize  + pixel.x;
                    buffer[ 4 * offset + 0 ] = 255;
                    buffer[ 4 * offset + 1 ] = 255;
                    buffer[ 4 * offset + 2] =  255;
                    buffer[ 4 * offset + 3 ] = greyLevel;
                }
            }

            glyphCoord.x++;
            if( glyphCoord.x >= mAtlasSize )
            {
                glyphCoord.x = 0;
                glyphCoord.y++;
            }
        }

        texture->LoadFromPixels( buffer, { bufferPixelSize,bufferPixelSize  }, 1 );
        return texture;
    }
}