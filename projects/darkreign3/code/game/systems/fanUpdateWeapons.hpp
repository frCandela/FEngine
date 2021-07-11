#pragma once

#include "game/components/fanUnit.hpp"
#include "game/components/fanTerrainAgent.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SFireWeapons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit, Weapon, TerrainAgent>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Fixed _delta )
        {
            auto unitIt      = _view.begin<Unit>();
            auto agentIt     = _view.begin<TerrainAgent>();
            auto weaponIt    = _view.begin<Weapon>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt, ++agentIt, ++weaponIt)
            {
                Unit        & unit      = *unitIt;
                Weapon      & weapon    = *weaponIt;
                TerrainAgent& agent     = *agentIt;

                weapon.mTimeAccumulator += _delta;

                const Fixed timePerBullet = 1 / weapon.mRateOfFire;
                if( unit.mState == Unit::Attack && unit.HasTarget() && agent.TargetIsInRange() && weapon.mTimeAccumulator >= timePerBullet )
                {
                    weapon.mTimeAccumulator -= timePerBullet;
                    if( unit.mFireDelegate )
                    {
                        ( *unit.mFireDelegate )( _world, unitIt.GetEntity() );
                    }
                }

                weapon.mTimeAccumulator = Fixed::Min( weapon.mTimeAccumulator, timePerBullet );
            }
        }
    };
}