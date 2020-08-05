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

        glm::ivec2 mPosition;

        static void Update( InputMouse& _mouse, const glm::ivec2 _sceenPos );
    };
}