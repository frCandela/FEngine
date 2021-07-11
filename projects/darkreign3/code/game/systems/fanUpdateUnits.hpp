#pragma once

#include "game/components/fanUnit.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "game/components/fanHealth.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateUnitsData : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world ) { return _world.GetSignature<Unit, TerrainAgent>(); }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto unitIt  = _view.begin<Unit>();
            auto agentIt = _view.begin<TerrainAgent>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt, ++agentIt )
            {
                Unit        & unit           = *unitIt;
                TerrainAgent& agent          = *agentIt;

                if( unit.HasTarget() && !_world.HandleExists( unit.mLastOrder.mTargetEntity ) )
                {
                    unit.mLastOrder.mTargetEntity = 0;
                }

                if( unit.HasTarget() )
                {
                    Transform& transformTarget = _world.GetComponent<Transform>( _world.GetEntity( unit.mLastOrder.mTargetEntity ) );
                    agent.mTargetPosition = transformTarget.mPosition;
                }
                agent.mSqrDistanceFromTarget = Vector3::SqrDistance( agent.mPositionOnTerrain, agent.mTargetPosition );
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateUnitsState : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit, TerrainAgent, TagUnitStateNeedsUpdate>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto unitIt  = _view.begin<Unit>();
            auto agentIt = _view.begin<TerrainAgent>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt, ++agentIt )
            {
                Unit        & unit  = *unitIt;
                TerrainAgent& agent = *agentIt;

                ApplyOrder( unit.mLastOrder, unit, agent );

                _world.RemoveTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );

                switch( unit.mState )
                {
                    case Unit::Wait:
                        break;
                    case Unit::Move:
                        if( agent.TargetIsInRange() )
                        {
                            if( unit.mLastOrder.mType == UnitOrder::Attack && unit.HasTarget() )
                            {
                                unit.mState = Unit::Attack;
                            }
                            else
                            {
                                unit.mState = Unit::Wait;
                            }
                        }
                        else
                        {
                            // keep moving
                        }
                        break;
                    case Unit::Attack:
                        if( !unit.HasTarget() ) // target is dead
                        {
                            unit.mState = Unit::Move;
                        }
                        else if( !agent.TargetIsInRange() )
                        {
                            unit.mState = Unit::Move;
                        }
                        else
                        {
                            // keep attacking
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        static bool ApplyOrder( UnitOrder& _order, Unit& _unit, TerrainAgent& _agent )
        {
            if( !_order.mWasExecuted )
            {
                _order.mWasExecuted = true;
                switch( _order.mType )
                {
                    case UnitOrder::Attack:
                    case UnitOrder::Move:
                        _unit.mState                  = Unit::Move;
                        _agent.mTargetPosition        = _order.mTargetPosition;
                        _agent.mSqrDistanceFromTarget = Fixed::sMaxValue;
                        return true;
                    default:
                        fanAssert( false );
                        return false;
                }
            }
            return false;
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SGiveOrderToSelectedUnits : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit>() | _world.GetSignature<TagSelected>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const UnitOrder& _order )
        {
            fanAssert( _order.mType != UnitOrder::None );
            auto unitIt = _view.begin<Unit>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt )
            {
                _world.AddTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );
                Unit& unit = *unitIt;
                unit.mLastOrder = _order;
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SPlayUnitState : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit, TerrainAgent>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto unitIt  = _view.begin<Unit>();
            auto agentIt = _view.begin<TerrainAgent>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt, ++agentIt )
            {
                Unit        & unit  = *unitIt;
                TerrainAgent& agent = *agentIt;

                switch( unit.mState )
                {
                    case Unit::Wait:
                        agent.mState = TerrainAgent::Stay;
                        break;
                    case Unit::Move:
                        agent.mState = TerrainAgent::Move;
                        agent.mRange = unit.mLastOrder.mType == UnitOrder::Attack ? unit.mAttackRange : 2;
                        break;
                    case Unit::Attack:
                        agent.mState = TerrainAgent::Face;
                        agent.mRange = unit.mAttackRange;
                        if( unit.mLastOrder.mTargetEntity == 0 ) // enemy dead or out of sight
                        {
                            _world.AddTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );
                        }
                        else if( agent.mSqrDistanceFromTarget > unit.mAttackRange * unit.mAttackRange ) // out of range
                        {
                            _world.AddTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );
                        }
                        break;
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateDeadUnits : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit, Health>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto unitIt   = _view.begin<Unit>();
            auto healthIt = _view.begin<Health>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt, ++healthIt )
            {
                Health& health = *healthIt;
                Unit  & unit   = *unitIt;
                if( health.mHealth <= 0 )
                {
                    _world.Kill( healthIt.GetEntity() );
                    if( unit.mDeathDelegate != nullptr )
                    {
                        ( *unit.mDeathDelegate )( _world, unitIt.GetEntity() );
                    }
                }
            }
        }
    };
}