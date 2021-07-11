#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include <vector>
#include "core/math/fanVector3.hpp"

namespace fan
{
    class EcsWorld;
    struct Transform;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct AIWorld : public EcsSingleton
    {
    ECS_SINGLETON( AIWorld )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        static void UpdateEnemiesLists( EcsWorld& _world );
        EcsHandle FindEnemyInRange( const Vector3& _position, const Fixed _range );

        struct EnemyInfo
        {
            EcsHandle mHandle;
            Transform* mTransform;
        };
        std::vector<EnemyInfo> mEnemies; // built every frame before AI logic
    };
}