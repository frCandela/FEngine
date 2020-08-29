#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <vector>
#include <map>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/vec2.hpp"
WARNINGS_POP()
#include "render/resources/fanTexture.hpp"

namespace fan
{
    class TextureManager;

    //========================================================================================================
    //========================================================================================================
    class Font
    {
    public:
        struct Glyph {
            glm::ivec2 mUVPos;   // pixels
            glm::ivec2 mSize;  // pixels
            glm::ivec2 mBearing; // pixels
            long       mAdvance; // 1/64th pixels
        };

        struct Atlas
        {
            int                       mGlyphSize = 0;   // max size of the side of a glyph in pixels
            int                       mSize = 0;        // number of glyph per row/col
            std::map<uint32_t, Glyph> mGlyphs;
            Texture*                  mTexture;

            int GetPixelSize() const { return mGlyphSize * mSize ; }
            const Glyph& GetGlyph( const uint32_t _codepoint ) const
            {
                auto it = mGlyphs.find( _codepoint );
                if( it == mGlyphs.end() )
                {
                    std::vector<uint32_t> defaultUnicode;
                    Font::ToUTF8( "?", defaultUnicode );
                    it = mGlyphs.find( defaultUnicode[0] );
                }
                //fanAssert( it != mGlyphs.end() );
                return it->second;
            }
        };

        Font(){}
        ~Font(){ FT_Done_Face( mFace ); }
        bool LoadFont( const std::string _path );
        const Atlas* GenerateAtlas( TextureManager& _textureManager,  int _height );
        const Atlas* FindAtlas( const int _height );

        static void ToUTF8( const std::string& _str, std::vector<uint32_t >& _outUnicode );
        static bool InitFreetype();
        static void ClearFreetype();

    private:
        static FT_Library   sFreetypeLib;
        FT_Face             mFace;
        std::vector<Atlas>  mAtlases;
    };
}