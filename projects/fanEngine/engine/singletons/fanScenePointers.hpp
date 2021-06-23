#pragma once

#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
    class EcsWorld;
    struct SceneNode;
    struct Scene;
    class Prefab;
    struct ComponentPtrBase;

    //==================================================================================================================================================================================================
    // Contains all scene pointers that need to be resolved
    // unresolved pointers appear when loading scenes or instancing prefabs
    //==================================================================================================================================================================================================
    struct ScenePointers : public EcsSingleton
    {
    ECS_SINGLETON( ScenePointers )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        std::vector<ComponentPtrBase*> mUnresolvedComponentPtr;

        static void Clear( ScenePointers& _scenePointers );
        static void ResolveComponentPointers( EcsWorld& _world, const uint32_t _idOffset );
    };
}