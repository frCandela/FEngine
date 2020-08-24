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
            return _world.GetSignature<UITransform>() |
                    _world.GetSignature<UIRenderer>() |
                    _world.GetSignature<UIText>();
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
                UITransform& transform = *transformIt;
                UIRenderer&  renderer = *rendererIt;
                UIText&      text = *textIt;

                (void)transform;

                if( !text.mModified ){ continue; }
                text.mModified = false;

                if( text.mText.empty() )
                {
                    renderer.mMesh2D = nullptr;
                    continue;
                }

                std::vector<UIVertex> vertices;
                const float           atlasPixelSize = (float)font.GetAtlasSize();

                std::vector<uint32_t> unicode;
                Font::ToUTF8( text.mText, unicode );
                long          x = 0;
                for( uint32_t codepoint : unicode )
                {
                    const Font::Glyph& glyph = font.GetGlyph( codepoint );

                    glm::vec2 uvPos  = glm::vec2( glyph.mUVPos ) / atlasPixelSize;
                    glm::vec2 uvSize = glm::vec2( glyph.mUVSize ) / atlasPixelSize;
                    glm::vec2 offset = 2.f * ( glm::vec2( x / 64, 0 ) +
                                               glm::vec2( glyph.mBearing.x, -glyph.mBearing.y ) ) /
                                       atlasPixelSize;
                    glm::vec2 pos    = 2.f * uvSize;

                    vertices.push_back( UIVertex( offset + glm::vec2( pos.x, +0.f ),
                                                  uvPos + glm::vec2( uvSize.x, 0 ) ) );
                    vertices.push_back( UIVertex( offset + glm::vec2( +0.f, +0.f ), uvPos ) );
                    vertices.push_back( UIVertex( offset + pos, uvPos + uvSize ) );
                    vertices.push_back( UIVertex( offset + glm::vec2( +0.f, +0.f ), uvPos ) );
                    vertices.push_back( UIVertex( offset + glm::vec2( +0.f, pos.y ),
                                                  uvPos + glm::vec2( 0, uvSize.y ) ) );
                    vertices.push_back( UIVertex( offset + pos, uvPos + uvSize ) );

                    x += glyph.mAdvance;
                }

                text.mMesh2D->LoadFromVertices( vertices );
                renderer.mMesh2D = text.mMesh2D;
            }
        }
    };
}