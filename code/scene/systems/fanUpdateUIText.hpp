#include "ecs/fanEcsSystem.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUIText.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/singletons/fanRenderResources.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct SUpdateUIText : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return  _world.GetSignature<UITransform>() |
                    _world.GetSignature<UIRenderer>() |
                    _world.GetSignature<UIText>() |
                    _world.GetSignature<TagUIModified>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderResources& resources = _world.GetSingleton<RenderResources>();
            Font& font = *resources.mFont;

            auto textIt = _view.begin<UIText>();
            auto rendererIt = _view.begin<UIRenderer>();
            auto transformIt = _view.begin<UITransform>();
            for( ; transformIt != _view.end<UITransform>(); ++transformIt, ++rendererIt, ++textIt )
            {
                _world.RemoveTag<TagUIModified>( textIt.GetEntity() );

                UITransform& transform = *transformIt;
                UIRenderer&  renderer = *rendererIt;
                UIText&      text = *textIt;

                if( text.mText.empty() )
                {
                    renderer.mMesh2D = nullptr;
                    renderer.mTexture = nullptr;
                    continue;
                }

                const Font::Atlas * atlas = font.FindAtlas( text.mSize );
                if( atlas == nullptr )
                {
                    atlas = font.GenerateAtlas( *resources.mTextureManager, text.mSize );
                    fanAssert( atlas != nullptr );
                }

                std::vector<uint32_t> unicode;
                Font::ToUTF8( text.mText, unicode );

                std::vector<const Font::Glyph*> glyphs;
                for( uint32_t codePoint : unicode ) { glyphs.push_back( &atlas->GetGlyph( codePoint ) ); }

                glm::ivec2 textOffset;
                CalculateTextOffsetAndSize( glyphs, textOffset, transform.mSize );

                std::vector<UIVertex> vertices;
                const float           atlasPixelSize = (float)atlas->GetPixelSize();
                const glm::vec2       scale          = glm::vec2( atlasPixelSize ) /
                                                       glm::vec2( transform.mSize );
                long          x = 0;
                for( int i = 0; i < glyphs.size(); i++ )
                {
                    const Font::Glyph& glyph = *glyphs[i];

                    glm::vec2 uvPos  = glm::vec2( glyph.mUVPos ) / atlasPixelSize;
                    glm::vec2 uvSize = glm::vec2( glyph.mSize ) / atlasPixelSize;
                    glm::vec2 pos    = 2.f * ( glm::vec2( -textOffset) +
                                               glm::vec2( x / 64, 0 ) +
                                               glm::vec2( glyph.mBearing.x, -glyph.mBearing.y ) ) /
                                       atlasPixelSize * scale;
                    glm::vec2 size   = 2.f * uvSize * scale;

                    // creates the quad
                    vertices.push_back( UIVertex( pos + glm::vec2( size.x, +0.f ),
                                                  uvPos + glm::vec2( uvSize.x, 0 ) ) );
                    vertices.push_back( UIVertex( pos + glm::vec2( +0.f, +0.f ), uvPos ) );
                    vertices.push_back( UIVertex( pos + size, uvPos + uvSize ) );
                    vertices.push_back( UIVertex( pos + glm::vec2( +0.f, +0.f ), uvPos ) );
                    vertices.push_back( UIVertex( pos + glm::vec2( +0.f, size.y ),
                                                  uvPos + glm::vec2( 0, uvSize.y ) ) );
                    vertices.push_back( UIVertex( pos + size, uvPos + uvSize ) );

                    x += glyph.mAdvance;
                }

                text.mMesh2D->LoadFromVertices( vertices );
                renderer.mMesh2D = text.mMesh2D;
                renderer.mTexture = atlas->mTexture;
            }
        }

        static void CalculateTextOffsetAndSize( std::vector<const Font::Glyph*> _glyphs,
                                                glm::ivec2& _outOffset,
                                                glm::ivec2& _outSize )
        {
            _outOffset = glm::ivec2( _glyphs[0]->mBearing.x, 0);
            glm::ivec2 max(0,0);
            for( int i = 0; i < _glyphs.size(); i++ )
            {
                const Font::Glyph& glyph = *_glyphs[i];
                max.x += glyph.mAdvance;
                _outOffset.y = std::min( _outOffset.y, -glyph.mBearing.y );
                max.y        = std::max( max.y, glyph.mSize.y - glyph.mBearing.y );
            }
            const Font::Glyph& lastGlyph = **_glyphs.rbegin();
            max.x -= lastGlyph.mAdvance;
            max.x += 64 * ( lastGlyph.mBearing.x + lastGlyph.mSize.x );
            max.x /= 64;

            _outSize = max - _outOffset;
        }
    };
}