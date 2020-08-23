#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/vec2.hpp"
WARNINGS_POP()
#include "render/resources/fanTexture.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class Font
    {
    public:
        Font(){}
        ~Font(){ FT_Done_Face( mFace ); }
        bool LoadFont( const std::string _path );
        bool SetHeight( const int _pixelHeight );
        Texture* GenerateAtlas();
        bool LoadChar( const char _char );

        static bool InitFreetype();
        static void ClearFreetype();
    private:
        struct Glyph {
            int        mTextureID;
            glm::ivec2 mSize;    // pixels
            glm::ivec2 mBearing; // pixels
            long       mAdvance; // 1/64th pixels
        };

        static FT_Library   sFreetypeLib;
        static const int    sNumAsciiCharacters = 128;
        static const int    sSizeAtlas = 16;
        static_assert( sSizeAtlas * sSizeAtlas >= sNumAsciiCharacters );

        FT_Face                 mFace;
        std::map<char, Glyph>   mGlyphs;
        int                     mPixelHeight = 0;
    };
}