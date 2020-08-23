#include "ecs/fanEcsSystem.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUIText.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"

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

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto rendererIt = _view.begin<UIRenderer>();
            auto transformIt = _view.begin<UITransform>();

            for( ; transformIt != _view.end<UITransform>(); ++transformIt, ++rendererIt )
            {
                UITransform& transform = *transformIt;
                UIRenderer& renderer = *rendererIt;

                (void) transform;
                (void) renderer;
            }
        }
    };
}