#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct InputMouse : public EcsSingleton
    {
        ECS_SINGLETON( InputMouse )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );

        enum Button
        {
            button1, button2, button3, button4, button5, button6, button7, button8, count,
            buttonLeft = button1,
            buttonRight = button2,
            buttonMiddle = button3
        };

        glm::ivec2 mPosition;
        bool       mPressed[ Button::count ];
        bool       mDown   [ Button::count];

        static void Update( InputMouse& _mouse, const glm::ivec2 _screenPos );
    };
}