#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include "core/fanAssert.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanSlot.hpp"

namespace fan
{
    struct EcsComponent;
    class EcsWorld;

    //========================================================================================================
    // A signal is used for communication between objects ( ~similar to qt Signals & Slots )
    //========================================================================================================
    template< typename ...Args > struct Signal
    {
        //================================================================
        //================================================================
        struct Connection
        {
            using Lambda = std::function<void( Args... )>;

            Lambda mLambda;
            size_t mID;
        };

        template< typename _Object >
        void Connect( void( _Object::* _method )( Args... ), _Object* _object );

        template< typename _ComponentType >
        void Connect( void( _ComponentType::* _method )( Args... ),
                      EcsWorld& _world,
                      const EcsHandle _handle );

        bool Connect(  EcsWorld& _world, SlotPtr& _slotPtr );

        void Emmit( Args... _args );
        void Clear();
        int ConnectionsCount() const { return (int)mConnections.size(); }
        void Disconnect( const size_t _ID );
        int GetType() const{ return TemplateType<Args...>::Type(); }

    private:
        std::vector<Connection> mConnections;
    };

    //========================================================================================================
    //========================================================================================================
    template< typename... Args >
    template< typename _Object >
    void Signal<Args...>::Connect( void (_Object::* _method)( Args... ), _Object* _object )
    {
        static_assert( !std::is_base_of<EcsComponent, _Object>::value );

        Connection connection;
        connection.mID     = size_t( _object );
        connection.mLambda = [_method, _object]( Args... _args )
        {
            ( ( *_object ).*( _method ) )( _args... );
        };
        mConnections.push_back( connection );
    }

    //========================================================================================================
    //========================================================================================================
    template< typename... Args >
    template< typename _ComponentType >
    void Signal<Args...>::Connect( void (_ComponentType::* _method)( Args... ),
                                   EcsWorld& _world,
                                   EcsHandle _handle )
    {
        static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
        fanAssert( _handle != 0 );

        Connection connection;
        connection.mID     = _handle;
        connection.mLambda = [_method, &_world, _handle]( Args... _args )
        {
            _ComponentType& component = _world.GetComponent<_ComponentType>( _world.GetEntity( _handle ) );
            ( ( component ).*( _method ) )( _args... );
        };
        mConnections.push_back( connection );
    }
    //========================================================================================================
    //========================================================================================================
    template< typename... Args >
    bool Signal<Args...>::Connect( EcsWorld& _world, SlotPtr& _slotPtr )
    {
        if( _slotPtr.GetArgsType() != GetType()) { return false; }

        const SlotPtr::SlotCallData * _callData = &_slotPtr.Data();

        Connection connection;
        connection.mID      = (size_t)_callData;
        connection.mLambda  = [ &_world, _callData ]( Args... _args )
        {
            if( _callData->mSlot == nullptr ){ return; }

            Slot<Args...>* slot = static_cast< Slot<Args...>* >(_callData->mSlot);
            switch( slot->mTargetType )
            {
                case SlotBase::TargetType::Component:
                    if( _callData->mHandle != 0  )
                    {
                        EcsEntity entity = _world.GetEntity( _callData->mHandle );
                        EcsComponent& component = _world.GetComponent( entity, _callData->mType );
                        (*slot->mMethod.mMethodComponent )( component, _args...);
                    }
                break;
                case  SlotBase::TargetType::Singleton:{
                    EcsSingleton& singleton = _world.GetSingleton( _callData->mType );
                    (*slot->mMethod.mMethodSingleton )( singleton, _args...);
                } break;
                default :
                    fanAssert( false );
                    break;
            }
        };

        mConnections.push_back( connection );
        return true;
    }

    //========================================================================================================
    //========================================================================================================
    template< typename... Args >
    void Signal<Args...>::Emmit( Args... _args )
    {
        for( Connection connection : mConnections )
        {
            connection.mLambda( _args... );
        }
    }

    //========================================================================================================
    //========================================================================================================
    template< typename... Args >
    void Signal<Args...>::Clear()
    {
        mConnections.clear();
    }

    //========================================================================================================
    //========================================================================================================
    template< typename... Args >
    void Signal<Args...>::Disconnect( const size_t _ID )
    {
        for( int i = (int)mConnections.size() - 1; i >= 0; i-- )
        {
            if( mConnections[i].mID == _ID )
            {
                mConnections.erase( mConnections.begin() + i );
            }
        }
    }
}