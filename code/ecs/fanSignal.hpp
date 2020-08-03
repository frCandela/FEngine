#pragma once

#include <vector>
#include <functional>
#include <cassert>
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    struct EcsComponent;
    class EcsWorld;

    //========================================================================================================
    // A signal is used for communication between objects ( similar to qt Signals & Slots )
    //
    // ex :
    // struct MyClass {void TestFunction( int ) {} {
    // MyClass test;
    // Signal<int> signal;
    // signal.Connect( &MyClass::TestFunction, &test );
    // signal.Emmit( 42 );
    //
    // Warning : The pointer to the connected object (_object)
    // must stay valid during the whole lifetime of the signal
    // When connecting to EcsComponents, you must pass the
    // EcsWorld and an EcsHandle of your component to the Connect method.
    //========================================================================================================
    template< typename ...Args > struct Signal
    {
        //================================================================
        //================================================================
        struct Connection
        {
            using Lambda = std::function<void( Args... )>; // lambda

            Lambda mLambda;
            size_t mID;
        };

        //================================================================
        //================================================================
        template< typename _Object >
        void Connect( void( _Object::* _method )( Args... ), _Object* _object )
        {
            static_assert( !std::is_base_of<EcsComponent, _Object>::value );

            Connection connection;
            connection.mID     = size_t( _object );
            connection.mLambda = [_method, _object]( Args... _args )
            {
                ( ( *_object ).*
                  ( _method ) )( _args... );
            };
            mConnections.push_back( connection );
        }

        //================================================================
        //================================================================
        template< typename _ComponentType >
        void
        Connect( void( _ComponentType::* _method )( Args... ), EcsWorld& _world, const EcsHandle _handle )
        {
            static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
            assert( _handle != 0 );

            Connection connection;
            connection.mID     = _handle;
            connection.mLambda = [_method, &_world, _handle]( Args... _args )
            {
                _ComponentType& component = _world.GetComponent<_ComponentType>( _world.GetEntity( _handle ) );
                ( ( component ).*( _method ) )( _args... );
            };
            mConnections.push_back( connection );
        }

        //================================================================
        //================================================================
        void Emmit( Args... _args )
        {
            for( Connection connection : mConnections )
            {
                connection.mLambda( _args... );
            }
        }

        //================================================================
        //================================================================
        void Clear()
        {
            mConnections.clear();
        }

        //================================================================
        //================================================================
        void Disconnect( const size_t _ID )
        {
            for( int i = (int)mConnections.size() - 1; i >= 0; i-- )
            {
                if( mConnections[i].mID == _ID )
                {
                    mConnections.erase( mConnections.begin() + i );
                }
            }
        }

    private:
        std::vector<Connection> mConnections;
    };
}