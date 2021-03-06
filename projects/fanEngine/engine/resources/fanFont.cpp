#include "fanFont.hpp"

#include "fanDisableWarnings.hpp"

WARNINGS_SFML_UTF_PUSH()
#include "SFML/System/Utf.hpp"

WARNINGS_POP()
#include "core/fanDebug.hpp"
#include "core/fanPath.hpp"
#include "core/resources/fanResources.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Font::Font( FT_Library _ftLib, const std::string _path )
    {
        if( FT_New_Face( _ftLib, Path::Normalize( _path ).c_str(), 0, &mFace ) )
        {
           Debug::Error() << "Freetype: Failed to load font " << _path << Debug::Endl();
            return;
        }

        if( FT_Select_Charmap( mFace, FT_ENCODING_UNICODE ) )
        {
            Debug::Error() << "Freetype: font " << _path << " has no unicode charmap" << Debug::Endl();
            return;
        }

        mPath = _path;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Font::ToUTF8( const std::string& _str, std::vector<uint32_t>& _outUnicode )
    {
        _outUnicode.clear();
        std::string::const_iterator it = _str.begin();
        while( it != _str.end() )
        {
            uint32_t codepoint;
            it = sf::Utf8::decode( it, _str.end(), codepoint );
            _outUnicode.push_back( codepoint );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const Font::Glyph& Font::Atlas::GetGlyph( const uint32_t _codePoint ) const
    {
        auto it = mGlyphs.find( _codePoint );
        if( it == mGlyphs.end() )
        {
            std::vector<uint32_t> defaultUnicode;
            Font::ToUTF8( "?", defaultUnicode );
            it = mGlyphs.find( defaultUnicode[0] );
        }
        fanAssert( it != mGlyphs.end() );
        return it->second;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    int FindAtlasSize( const size_t _numElements )
    {
        size_t size = 1;
        while( size * size < _numElements )
        {
            size *= 2;
        }
        return (int)size;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const Font::Atlas* Font::FindAtlas( const int _height )
    {
        for( Atlas& atlas : mAtlases )
        {
            if( atlas.mGlyphSize == _height )
            {
                return &atlas;
            }
        }
        return nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const Font::Atlas* Font::GenerateAtlas( Resources& _resources, const int _height )
    {
        fanAssert( FindAtlas( _height ) == nullptr );
        fanAssert( _height > 0 );
        if( FT_Set_Pixel_Sizes( mFace, 0, _height ) != 0 )
        {
            Debug::Error() << "Font::GenerateAtlas invalid pixel height" << _height << Debug::Endl();
            return nullptr;
        }
        fanAssert( mFace->charmap != nullptr );

        std::vector<FT_ULong> unicodeCharacters;
        FT_UInt               index;
        FT_ULong              character = FT_Get_First_Char( mFace, &index );
        while( true )
        {
            unicodeCharacters.push_back( character );
            character = FT_Get_Next_Char( mFace, character, &index );
            if( !index ){ break; }
        }

        Atlas atlas;
        atlas.mSize      = FindAtlasSize( unicodeCharacters.size() );
        atlas.mGlyphSize = _height;

        const size_t bufferPixelSize = atlas.mGlyphSize * atlas.mSize;
        uint8_t* buffer = new uint8_t[bufferPixelSize * bufferPixelSize * 4];

        glm::ivec2 glyphCoord( 0, 0 );
        for( int   i   = 0; i < (int)unicodeCharacters.size(); i++ )
        {
            fanAssert( i < 256 );
            const unsigned long charcode   = unicodeCharacters[i];
            const unsigned      glyphIndex = FT_Get_Char_Index( mFace, charcode );
            if( FT_Load_Glyph( mFace, glyphIndex, FT_LOAD_DEFAULT ) != 0 )
            {
                Debug::Error( "font glyph failed to load" );
                continue;
            }

            if( FT_Render_Glyph( mFace->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
            {
                Debug::Error( "font glyph failed to load" );
                continue;
            }

            const glm::ivec2 glyphSize = glm::ivec2( mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows );
            if( glyphSize.x > atlas.mGlyphSize || glyphSize.y > atlas.mGlyphSize ){ continue; }

            const glm::ivec2 glyphOrigin  = atlas.mGlyphSize * glyphCoord;
            const glm::ivec2 glyphBearing = glm::ivec2( mFace->glyph->bitmap_left, mFace->glyph->bitmap_top );
            Glyph            glyph        = {
                    glyphOrigin,
                    glyphSize,
                    glyphBearing,
                    mFace->glyph->advance.x
            };
            atlas.mGlyphs[charcode] = glyph;

            for( int x = 0; x < glyph.mSize.x; x++ )
            {
                for( int y = 0; y < glyph.mSize.y; y++ )
                {
                    const size_t glyphLocalOffset = y * glyph.mSize.x + x;
                    const char   greyLevel        = mFace->glyph->bitmap.buffer[glyphLocalOffset];

                    glm::ivec2   pixel  = glyphOrigin + glm::ivec2( x, y );
                    const size_t offset = pixel.y * bufferPixelSize + pixel.x;
                    buffer[4 * offset + 0] = 255;
                    buffer[4 * offset + 1] = 255;
                    buffer[4 * offset + 2] = 255;
                    buffer[4 * offset + 3] = greyLevel;
                }
            }

            glyphCoord.x++;
            if( glyphCoord.x >= atlas.mSize )
            {
                glyphCoord.x = 0;
                glyphCoord.y++;
            }
        }

        Texture * atlasTexture = new Texture();
        atlasTexture->mIsGenerated = true;
        atlasTexture->LoadFromPixels( buffer, { bufferPixelSize, bufferPixelSize }, 1 );
        atlas.mTexture = _resources.Add<Texture>( atlasTexture, mPath + "size_" + std::to_string( _height ) );
        mAtlases.push_back( atlas );
        return &*mAtlases.rbegin();
    }
}