#pragma once

#include <vector>
#include <functional>
#include "core/fanAssert.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "ecs/fanSlot.hpp"

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

        void Connect( EcsWorld& _world,
                      const EcsHandle _handle,
                      uint32_t _type,
                      SlotBase* _slot );

        void Emmit( Args... _args );
        void Clear();
        void Disconnect( const size_t _ID );
        int GetType() const{ return TemplateType::Type<Args...>(); }

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
    void Signal<Args...>::Connect( EcsWorld& _world,
                                   const EcsHandle _handle,
                                   uint32_t _type,
                                   SlotBase* _slot )
    {
        fanAssert( _handle != 0 );
        fanAssert( _slot->GetType() == GetType() );
        Slot<Args...>* slot = static_cast< Slot<Args...>* >(_slot);

        Connection connection;
        connection.mID     = _handle;
        connection.mLambda  = [&_world, slot, _type, _handle]( Args... _args )
        {
            EcsComponent& component = _world.GetComponent( _world.GetEntity( _handle ), _type );
            //( component ).*( method ) )( _args... );
           (*slot->mMethod)( component, _args...);
        };
        mConnections.push_back( connection );
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