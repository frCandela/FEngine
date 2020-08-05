#include "ecs/fanEcsSystem.hpp"
#include "scene/components/ui/fanButtonUI.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/singletons/fanRenderWorld.hpp"

namespace fan
{
    class EcsWorld;

    //========================================================================================================
    //========================================================================================================
    struct SRaycastButtons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return	_world.GetSignature<Button>() | _world.GetSignature<TransformUI>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();

            auto buttonIt = _view.begin<Button>();
            auto transformUIIt = _view.begin<TransformUI>();
            for( ; buttonIt != _view.end<Button>(); ++buttonIt, ++transformUIIt )
            {
                Button& button = *buttonIt;
                TransformUI& transform = *transformUIIt;

                button.mIsHovered = ! button.mIsHovered;

                (void)button;
                (void)transform;
                (void)renderWorld;
            }
        }
    };
}