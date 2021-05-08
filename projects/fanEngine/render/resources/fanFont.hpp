#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <vector>
#include <map>
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "render/resources/fanTexture.hpp"

namespace fan
{
    class TextureManager;

    //========================================================================================================
    //========================================================================================================
    class Font : public Resource
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

            int          GetPixelSize() const { return mGlyphSize * mSize ; }
            const Glyph& GetGlyph( const uint32_t _codePoint ) const;
        };

        Font( FT_Library _ftLib, const std::string _path );
        ~Font(){ if( IsValid() ) { FT_Done_Face( mFace ); } }
        const Atlas* GenerateAtlas( TextureManager& _textureManager,  int _height );
        const Atlas* FindAtlas( const int _height );
        bool IsValid() const { return ! mPath.empty(); }

        static void ToUTF8( const std::string& _str, std::vector<uint32_t >& _outUnicode );

        FT_Face             mFace;
        std::vector<Atlas>  mAtlases;
        std::string         mPath;
    };
}