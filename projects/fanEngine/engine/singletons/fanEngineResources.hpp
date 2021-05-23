#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "engine/resources/fanCursors.hpp"

namespace fan
{
    class ResourceManager;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct EngineResources : public EcsSingleton
    {
    ECS_SINGLETON( EngineResources )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        void SetupResources( ResourceManager& _resourceManager );

        ResourceManager * mResources ;

        std::vector<Cursor> mCursors;
        Cursor* mCurrentCursor;
    };
}