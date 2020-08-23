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
        bool LoadChar( const char _char );
        void SetHeight( const int _pixelHeight );

        static bool InitFreetype();
        static void ClearFreetype();

        Texture * mTexture = nullptr;
    private:
        struct Character {
            int        mTextureID;
            glm::ivec2 mSize;    // pixels
            glm::ivec2 mBearing; // pixels
            long       mAdvance; // 1/64th pixels
        };

        static FT_Library   sFreetypeLib;
        FT_Face             mFace;
        std::map<char, Character> mCharacters;

    };
}