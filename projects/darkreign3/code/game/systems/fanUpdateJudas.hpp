#include "ecs/fanEcsSystem.hpp"
#include "game/units/fanJudas.hpp"
#include "game/components/fanTerrainAgent.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateJudasAnimation : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<TerrainAgent>()
                   | _world.GetSignature<Animator>()
                   | _world.GetSignature<Judas>();
        }

        static void Run( EcsWorld&, const EcsView& _view )
        {
            auto animatorIt = _view.begin<Animator>();
            auto agentIt    = _view.begin<TerrainAgent>();
            auto judasIt    = _view.begin<Judas>();
            for( ; agentIt != _view.end<TerrainAgent>(); ++agentIt, ++judasIt, ++animatorIt )
            {
                TerrainAgent& agent    = *agentIt;
                Judas       & judas    = *judasIt;
                Animator    & animator = *animatorIt;
                const Fixed distanceWalk = 10;
                if( agent.mState == TerrainAgent::State::Stay )
                {
                    animator.mAnimation = judas.mAnimIdle;
                }
                else
                {
                    if( agent.mSqrDistanceFromDestination < distanceWalk * distanceWalk || Fixed::Abs( agent.mForwardAngle ) > 30 )
                    {
                        animator.mAnimation = judas.mAnimWalk;
                    }
                    else
                    {
                        animator.mAnimation = judas.mAnimRun;
                    }
                }
            }
        }
    };
}