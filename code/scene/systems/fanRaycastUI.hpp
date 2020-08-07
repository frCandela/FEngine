#include "ecs/fanEcsSystem.hpp"
#include "scene/components/ui/fanButtonUI.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/singletons/fanInputMouse.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct SHoverButtons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return	_world.GetSignature<Button>() | _world.GetSignature<TransformUI>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            Mouse2& mouse = _world.GetSingleton<Mouse2>();

            auto buttonIt = _view.begin<Button>();
            auto transformUIIt = _view.begin<TransformUI>();
            for( ; buttonIt != _view.end<Button>(); ++buttonIt, ++transformUIIt )
            {
                Button& button = *buttonIt;
                TransformUI& transform = *transformUIIt;

                button.mIsHovered = mouse.mPosition.x >= transform.mPosition.x &&
                                    mouse.mPosition.y >= transform.mPosition.y &&
                                    mouse.mPosition.x < transform.mPosition.x + transform.mSize.x &&
                                    mouse.mPosition.y < transform.mPosition.y + transform.mSize.y;
                button.mPressed = button.mIsHovered && mouse.mPressed[0];
            }
        }
    };
}