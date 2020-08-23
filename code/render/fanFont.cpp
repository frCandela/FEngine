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
    bool Font::SetHeight( const int _pixelHeight )
    {
        if(  FT_Set_Pixel_Sizes( mFace, 0, _pixelHeight ) != 0 )
        {
            Debug::Error() << "Font::SetHeight " << _pixelHeight << " failed" << Debug::Endl();
            return false;
        }
        mPixelHeight = _pixelHeight;
        return true;
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

        return true;
    }

    //========================================================================================================
    //========================================================================================================
    Texture* Font::GenerateAtlas()
    {
        fanAssert( mPixelHeight != 0 );

        const size_t bufferPixelSize = mPixelHeight * sSizeAtlas;
        uint8_t * buffer = new uint8_t[ bufferPixelSize * bufferPixelSize * 4 ];
        Texture * texture = new Texture();

        glm::ivec2 glyphCoord(0,0);
        for ( int  i = 0; i < 256; i++)
        {
            fanAssert( i < 256 );
            char character = (char)i;
            if( ! LoadChar( character ) )
            {
                continue;
            }

            Glyph glyph = {
                    character,
                    glm::ivec2(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows),
                    glm::ivec2(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
                    mFace->glyph->advance.x
            };
            mGlyphs.insert(std::pair<char, Glyph>( character, glyph) );

            fanAssert( glyph.mSize.x <= mPixelHeight);
            fanAssert( glyph.mSize.y <= mPixelHeight);

            const glm::ivec2 glyphOrigin = mPixelHeight * glyphCoord;
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
            if( glyphCoord.x >= sSizeAtlas )
            {
                glyphCoord.x = 0;
                glyphCoord.y++;
            }
        }

        texture->LoadFromPixels( buffer, { bufferPixelSize,bufferPixelSize  }, 1 );
        return texture;
    }
}