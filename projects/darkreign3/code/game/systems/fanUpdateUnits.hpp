#pragma once

#include "game/components/fanUnit.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "game/singletons/fanAIWorld.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
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

                Debug::Log( "state update" );
                _world.RemoveTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );

                switch( unit.mState )
                {
                    case Unit::Wait:
                        break;
                    case Unit::Move:
                        if( agent.mSqrDistanceFromDestination < agent.mRange * agent.mRange ) // arrived to destination
                        {
                            if( unit.mLastOrder.mType == UnitOrder::Attack && unit.mLastOrder.mTarget != 0 ) // go back to attacking
                            {
                                unit.mState = Unit::Attack;
                                agent.mState = TerrainAgent::Face;
                            }
                            else
                            {
                                unit.mState = Unit::Wait;
                                agent.mState = TerrainAgent::Stay;
                            }
                        }
                        else
                        {
                            Debug::Log("prout");
                        }
                        break;
                    case Unit::Attack:
                        if( unit.mLastOrder.mTarget == 0 ) // target dead or out of sight
                        {
                            fanAssert( unit.mLastOrder.mType == UnitOrder::Attack );
                            unit.mLastOrder.mWasExecuted = false;
                            ApplyOrder( unit.mLastOrder, unit, agent );
                        }
                        else if( agent.mSqrDistanceFromDestination > unit.mAttackRange * unit.mAttackRange ) // out of range
                        {
                            if( _world.HandleExists( unit.mLastOrder.mTarget ) )
                            {
                                fanAssert( unit.mLastOrder.mType == UnitOrder::Attack );
                                unit.mLastOrder.mWasExecuted = false;
                                unit.mLastOrder.mPosition    = _world.GetComponent<Transform>( _world.GetEntity( unit.mLastOrder.mTarget ) ).mPosition    ;
                                ApplyOrder( unit.mLastOrder, unit, agent );
                            }
                            else // target was killed
                            {
                                unit.mState = Unit::Wait;
                                agent.mState = TerrainAgent::Stay;
                            }
                        }
                        else
                        {
                            agent.mState = TerrainAgent::Face;
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
                Debug::Log( "order" );
                _order.mWasExecuted = true;
                switch( _order.mType )
                {
                    case UnitOrder::Attack:
                    case UnitOrder::Move:
                        _unit.mState                       = Unit::Move;
                        _agent.mState                      = TerrainAgent::Move;
                        _agent.mRange                      = _order.mType == UnitOrder::Attack ? _unit.mAttackRange : 2;
                        _agent.mSqrDistanceFromDestination = Fixed::sMaxValue;
                        _agent.mDestination                = _unit.mLastOrder.mPosition;
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

                if( unit.mLastOrder.mTarget != 0 && !_world.HandleExists( unit.mLastOrder.mTarget ) )
                {
                    unit.mLastOrder.mTarget = 0;
                }

                if( unit.mLastOrder.mTarget != 0 )
                {
                    Transform& transformTarget = _world.GetComponent<Transform>( _world.GetEntity( unit.mLastOrder.mTarget ) );
                    agent.mDestination = transformTarget.mPosition;
                }
                agent.mSqrDistanceFromDestination = Vector3::SqrDistance( agent.mPositionOnTerrain, agent.mDestination );

                switch( unit.mState )
                {
                    case Unit::Attack:
                        if( unit.mLastOrder.mTarget == 0 ) // enemy dead or out of sight
                        {
                            _world.AddTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );
                        }
                        else if( agent.mSqrDistanceFromDestination > unit.mAttackRange * unit.mAttackRange ) // out of range
                        {
                            _world.AddTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );
                        }
                        break;
                }
            }
        }
    };
}