#pragma once

#include "game/components/fanUnit.hpp"
#include "game/components/fanTerrainAgent.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SUpdateUnitsState : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Unit>()
                   | _world.GetSignature<TerrainAgent>()
                   | _world.GetSignature<TagUnitStateNeedsUpdate>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto unitIt  = _view.begin<Unit>();
            auto agentIt = _view.begin<TerrainAgent>();
            for( ; unitIt != _view.end<Unit>(); ++unitIt, ++agentIt )
            {
                Unit        & unit  = *unitIt;
                TerrainAgent& agent = *agentIt;
                _world.RemoveTag<TagUnitStateNeedsUpdate>( unitIt.GetEntity() );
                switch( unit.mState )
                {
                    case Unit::Wait:
                        if( !ApplyOrder( unit.mLastOrder, unit, agent ) )
                        {

                        }
                        break;
                    case Unit::Move:
                        if( ! ApplyOrder( unit.mLastOrder, unit, agent ) )
                        {
                            if( unit.mLastOrder.mType == UnitOrder::Move)
                            {
                                unit.mState = Unit::Wait;
                            }
                            else if( unit.mLastOrder.mType == UnitOrder::Attack)
                            {
                                unit.mState = Unit::Attack;
                            }
                        }
                        break;
                    case Unit::Attack:
                        if( ! ApplyOrder( unit.mLastOrder, unit, agent ) )
                        {

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
                    case UnitOrder::Move:
                    case UnitOrder::Attack:
                        _unit.mState        = Unit::Move;
                        _agent.mState       = TerrainAgent::Move;
                        _agent.mRange       = _order.mType == UnitOrder::Attack ? _unit.mAttackRange : 1;
                        _agent.mDestination = _unit.mLastOrder.mPosition;
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
                Debug::Log( "give order" );
            }
        }
    };
}