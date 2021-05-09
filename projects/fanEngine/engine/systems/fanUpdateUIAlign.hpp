#include "core/ecs/fanEcsSystem.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIAlign.hpp"
#include "engine/singletons/fanRenderWorld.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct SAlignUI : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UITransform>() |
                   _world.GetSignature<UIAlign>() |
                   _world.GetSignature<SceneNode>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();

            auto sceneNodeIt = _view.begin<SceneNode>();
            auto alignIt = _view.begin<UIAlign>();
            auto transformUIIt = _view.begin<UITransform>();
            for( ; alignIt != _view.end<UIAlign>(); ++alignIt, ++transformUIIt, ++sceneNodeIt )
            {
                UIAlign   &  align          = *alignIt;
                UITransform& childTransform = *transformUIIt;
                SceneNode& sceneNode = *sceneNodeIt;

                fanAssert( sceneNode.mParentHandle != 0 );
                EcsEntity parentEntity = _world.GetEntity(sceneNode.mParentHandle);
                UITransform* parentTransform = _world.SafeGetComponent<UITransform>( parentEntity );

                glm::ivec2 pPos;
                glm::ivec2 pSize;
                if( parentTransform == nullptr )
                {
                    pPos = glm::ivec2(0,0);
                    pSize = glm::ivec2( renderWorld.mTargetSize );
                }
                else
                {
                    pPos = parentTransform->mPosition;
                    pSize  = parentTransform->mSize;
                }

                const glm::ivec2& cSize = childTransform.mSize;
                glm::ivec2& cPos = childTransform.mPosition;

                // set position relative to the selected corner
                switch( align.mCorner )
                {
                    case UIAlign::TopLeft:
                        cPos = pPos;
                        break;
                    case UIAlign::TopRight:
                        cPos = glm::ivec2( pPos.x + pSize.x - cSize.x, pPos.y);
                        break;
                    case UIAlign::BottomLeft:
                        cPos = glm::ivec2( pPos.x, pPos.y + pSize.y - cSize.y);
                        break;
                    case UIAlign::BottomRight:
                        cPos = glm::ivec2( pPos.x + pSize.x - cSize.x, pPos.y + pSize.y - cSize.y );
                        break;
                    default: fanAssert(false); break;
                }

                // offset the position
                glm::vec2 offsetMultiplier = align.mUnitType == UIAlign::UnitType::Ratio  ?
                                            glm::vec2( pSize - cSize ) :
                                            glm::vec2( 1.f, 1.f );
                switch( align.mDirection )
                {
                    case UIAlign::Horizontal:
                        cPos.x += int( align.mOffset.x * offsetMultiplier.x );
                        break;
                    case UIAlign::Vertical:
                        cPos.y += int( align.mOffset.y * offsetMultiplier.y );
                        break;
                    case UIAlign::HorizontalVertical:
                        cPos += glm::ivec2 ( align.mOffset * offsetMultiplier );
                        break;
                }
            }
        }
    };
}