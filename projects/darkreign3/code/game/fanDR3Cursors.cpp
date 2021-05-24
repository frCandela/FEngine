#include "game/fanDR3Cursors.hpp"

#include "core/resources/fanResources.hpp"
#include "render/resources/fanTexture.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void DR3Cursors::Load( Resources& _resources )
    {
        Texture texture;
        texture.LoadFromFile( DR3Cursors::sIconsPath );
        constexpr glm::ivec2 tilemapSize = { 4, 4 };
        constexpr glm::ivec2 size        = { 32, 32 };
        fanAssert( tilemapSize.x * size.x == (int)texture.mExtent.width );
        fanAssert( tilemapSize.y * size.y == (int)texture.mExtent.height );

        struct CursorMapping
        {
            int        mType;
            glm::ivec2 mTileOffset;
            glm::ivec2 mHotspot;
        };

        std::vector<CursorMapping> cursorMappings = {
                { DR3Cursors::Move1,      { 0, 0 }, { 0,          0 } },
                { DR3Cursors::Move2,      { 1, 0 }, { 0,          0 } },
                { DR3Cursors::Move3,      { 3, 1 }, { 0,          0 } },
                { DR3Cursors::Mouse0,     { 2, 0 }, { 0,          0 } },
                { DR3Cursors::Mouse1,     { 3, 0 }, { 0,          0 } },
                { DR3Cursors::RallyPoint, { 0, 1 }, { size.x / 2, size.y } },
                { DR3Cursors::Nope,       { 2, 1 }, size / 2 },
                { DR3Cursors::Attack1,    { 0, 2 }, size / 2 },
                { DR3Cursors::Attack2,    { 1, 2 }, size / 2 },
                { DR3Cursors::Attack3,    { 2, 2 }, size / 2 },
                { DR3Cursors::Attack4,    { 3, 2 }, size / 2 },
        };

        for( CursorMapping mapping :  cursorMappings )
        {
            Cursor* cursor = new Cursor;

            unsigned char pixels[size.x * size.y * 4];
            glm::ivec2    position;
            for( position.x = 0; position.x < size.x; ++position.x )
            {
                for( position.y = 0; position.y < size.y; ++position.y )
                {
                    const glm::ivec2 sourcePosition = mapping.mTileOffset * size + position;
                    using GlmColor = glm::vec<4, unsigned char>;
                    const GlmColor& sourceColor = *(GlmColor*)( texture.mPixels + 4 * sourcePosition.x + 4 * texture.mExtent.width * sourcePosition.y );
                    GlmColor      & targetColor = *(GlmColor*)( pixels + 4 * position.x + 4 * size.x * position.y );
                    targetColor = sourceColor;
                }
            }
            cursor->Create( pixels, { size.x, size.y }, mapping.mHotspot );
            mCursors[mapping.mType] = _resources.Add<Cursor>( cursor, "cursor" + std::to_string( _resources.GetUniqueID() ) );
        }
        texture.FreePixels();
    }
}