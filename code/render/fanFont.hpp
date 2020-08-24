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
    //========================================================================================================
    //========================================================================================================
    class Font
    {
    public:
        struct Glyph {
            glm::ivec2 mUVPos;   // pixels
            glm::ivec2 mUVSize;  // pixels
            glm::ivec2 mBearing; // pixels
            long       mAdvance; // 1/64th pixels
        };

        Font(){}
        ~Font(){ FT_Done_Face( mFace ); }
        bool LoadFont( const std::string _path );
        bool SetHeight( const int _pixelHeight );
        Texture* GenerateAtlas();
        int GetAtlasSize() const { return mGlyphSize * mAtlasSize ; }
        const Glyph& GetGlyph( const uint32_t _codepoint ) const;

        static void ToUTF8( const std::string& _str, std::vector<uint32_t >& _outUnicode );
        static bool InitFreetype();
        static void ClearFreetype();

    private:
        static FT_Library   sFreetypeLib;

        int mAtlasSize = 0; // number of glyph per row/col
        int mGlyphSize = 0; // max size of the side of a glyph in pixels
        FT_Face mFace;
        std::map< uint32_t, Glyph>  mGlyphs;
    };
}