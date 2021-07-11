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
            return _world.GetSignature<Unit>() | _world.GetSignature<TerrainAgent>() | _world.GetSignature<Animator>() | _world.GetSignature<Judas>();
        }

        static void Run( EcsWorld&, const EcsView& _view, const Fixed _delta )
        {
            if( _delta <= 0 ){ return; }

            auto animatorIt = _view.begin<Animator>();
            auto agentIt    = _view.begin<TerrainAgent>();
            auto unitIt     = _view.begin<Unit>();
            auto judasIt    = _view.begin<Judas>();
            for( ; agentIt != _view.end<TerrainAgent>(); ++agentIt, ++judasIt, ++animatorIt, ++unitIt )
            {
                TerrainAgent& agent    = *agentIt;
                Judas       & judas    = *judasIt;
                Animator    & animator = *animatorIt;
                Unit        & unit     = *unitIt;
                const Fixed distanceWalk = 10;
                switch( unit.mState )
                {
                    case Unit::Wait:
                        animator.mAnimation = judas.mAnimIdle;
                        break;
                    case Unit::Move:
                        if( agent.mSqrDistanceFromTarget < distanceWalk * distanceWalk || Fixed::Abs( agent.mForwardAngle ) > 30 )
                        {
                            animator.mAnimation = judas.mAnimWalk;
                        }
                        else
                        {
                            animator.mAnimation = judas.mAnimRun;
                        }
                        break;
                    case Unit::Attack:
                        animator.mAnimation = judas.mAnimFire;
                        break;
                    default:
                        fanAssert( false );
                        break;
                }
            }
        }
    };
}