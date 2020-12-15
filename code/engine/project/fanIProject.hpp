#pragma once

#include "core/ecs/fanSignal.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class IProject
    {
    public:
        IProject(){}
        IProject( IProject const& ) = delete;
        IProject& operator=( IProject const& ) = delete;

        virtual void Init() = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Step( const float _delta ) = 0;
        virtual void UpdateRenderWorld() = 0;

        static void EcsIncludeEngine    ( EcsWorld& _world );
        static void EcsIncludePhysics ( EcsWorld& _world );
        static void EcsIncludeRender3D( EcsWorld& _world );
        static void EcsIncludeRenderUI( EcsWorld& _world );
        static void EcsIncludeNetworkClient ( EcsWorld& _world );
        static void EcsIncludeNetworkServer ( EcsWorld& _world );

        EcsWorld    mWorld;
        std::string mName;
    };
}