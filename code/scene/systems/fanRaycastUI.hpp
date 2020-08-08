#include "ecs/fanEcsSystem.hpp"
#include "scene/components/ui/fanButtonUI.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "core/fanDebug.hpp"

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
            Mouse& mouse = _world.GetSingleton<Mouse>();

            auto buttonIt = _view.begin<Button>();
            auto transformUIIt = _view.begin<TransformUI>();
            for( ; buttonIt != _view.end<Button>(); ++buttonIt, ++transformUIIt )
            {
                Button& button = *buttonIt;
                TransformUI& transform = *transformUIIt;

                button.mIsHovered = mouse.mLocalPosition.x >= transform.mPosition.x &&
                                    mouse.mLocalPosition.y >= transform.mPosition.y &&
                                    mouse.mLocalPosition.x < transform.mPosition.x + transform.mSize.x &&
                                    mouse.mLocalPosition.y < transform.mPosition.y + transform.mSize.y;
                if( button.mIsHovered && mouse.mPressed[Mouse::buttonLeft] ){ button.mIsPressed = true;  }
                else if( !mouse.mDown[Mouse::buttonLeft] )                  { button.mIsPressed = false; }
            }
        }
    };

    //========================================================================================================
    //========================================================================================================
    struct SHighlightButtons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return	_world.GetSignature<Button>() | _world.GetSignature<UIRenderer>();
        }

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto buttonIt = _view.begin<Button>();
            auto uiRendererIt = _view.begin<UIRenderer>();
            for( ; buttonIt != _view.end<Button>(); ++buttonIt, ++uiRendererIt )
            {
                Button& button = *buttonIt;
                UIRenderer& uiRenderer = *uiRendererIt;
                if( button.mIsPressed )     { uiRenderer.color = button.mColorPressed; }
                else if( button.mIsHovered ){ uiRenderer.color = button.mColorHovered; }
                else                        { uiRenderer.color = Color::White; }
            }
        }
    };
}