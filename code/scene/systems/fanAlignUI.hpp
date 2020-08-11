#include "ecs/fanEcsSystem.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUIAlign.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct SAlignUI : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UITransform>() | _world.GetSignature<UIAlign>();
        }

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto alignIt = _view.begin<UIAlign>();
            auto transformUIIt = _view.begin<UITransform>();
            for( ; alignIt != _view.end<UIAlign>(); ++alignIt, ++transformUIIt )
            {
                UIAlign   &  align          = *alignIt;
                UITransform& childTransform = *transformUIIt;

                if( align.mParent == nullptr ) { continue; }

                UITransform& parentTransform = *align.mParent;
                const glm::ivec2& pPos = parentTransform.mPosition;
                const glm::ivec2& pSize = parentTransform.mSize;
                const glm::ivec2& cSize = childTransform.mSize;
                switch( align.mCorner )
                {
                    case UIAlign::TopLeft:
                        childTransform.mPosition = parentTransform.mPosition;
                        break;
                    case UIAlign::TopRight:
                        childTransform.mPosition = glm::ivec2( pPos.x + pSize.x - cSize.x, pPos.y);
                        break;
                    case UIAlign::BottomLeft:
                        childTransform.mPosition = glm::ivec2( pPos.x, pPos.y + pSize.y - cSize.y);
                        break;
                    case UIAlign::BottomRight:
                        childTransform.mPosition = glm::ivec2( pPos.x + pSize.x - cSize.x,
                                                               pPos.y + pSize.y - cSize.y );
                        break;
                    default: fanAssert(false); break;
                }
            }
        }
    };
}