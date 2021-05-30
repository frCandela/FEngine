#include "game/fanDR3Cursors.hpp"

#include "core/resources/fanResources.hpp"
#include "render/resources/fanTexture.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/singletons/fanSelection.hpp"

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
                { DR3Cursor::Move1,      { 0, 0 }, { 0,          0 } },
                { DR3Cursor::Move2,      { 1, 0 }, { 0,          0 } },
                { DR3Cursor::Move3,      { 3, 1 }, { 0,          0 } },
                { DR3Cursor::Mouse0,     { 2, 0 }, { 0,          0 } },
                { DR3Cursor::Mouse1,     { 3, 0 }, { 0,          0 } },
                { DR3Cursor::RallyPoint, { 0, 1 }, { size.x / 2, size.y } },
                { DR3Cursor::Nope,       { 2, 1 }, size / 2 },
                { DR3Cursor::Attack1,    { 0, 2 }, size / 2 },
                { DR3Cursor::Attack2,    { 1, 2 }, size / 2 },
                { DR3Cursor::Attack3,    { 2, 2 }, size / 2 },
                { DR3Cursor::Attack4,    { 3, 2 }, size / 2 },
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    DR3Cursor PlayCursorAnim( const Time& _time, const Fixed _duration, const DR3Cursor* _cursors, const int _numCursors )
    {
        const int animFramesCount = ( _duration / _time.mLogicDelta ).ToInt();
        const int index           = _time.mFrameIndex % ( _numCursors * animFramesCount );
        const int animIndex       = index / animFramesCount;
        fanAssert( animIndex < _numCursors );
        return _cursors[animIndex];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    DR3Cursor DR3Cursors::GetCurrentCursor( const Fixed _delta, const Time& _time, const SelectionStatus& _selectionStatus )
    {
        if( _delta == 0 )
        {
            return DR3Cursor::Count;
        }

        if( _delta != 0 )
        {
            if( _selectionStatus.mNumSelected == 0 )
            {
                return _selectionStatus.mHoveringOverAlly ? DR3Cursor::Mouse0 : DR3Cursor::Mouse1;
            }
            else
            {
                if( _selectionStatus.mHoveringOverEnemy )
                {
                    static const DR3Cursor cursors[3] = { DR3Cursor::Attack3, DR3Cursor::Attack2, DR3Cursor::Attack1 };
                    return PlayCursorAnim( _time, FIXED( 0.13 ), cursors, 3 );
                }
                else
                {
                    static const DR3Cursor cursors[3] = { DR3Cursor::Move3, DR3Cursor::Move2, DR3Cursor::Move1 };
                    return PlayCursorAnim( _time, FIXED( 0.13 ), cursors, 3 );
                }
            }
        }
        return DR3Cursor::Count;
    }
}