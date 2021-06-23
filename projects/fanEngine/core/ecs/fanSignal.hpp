#pragma once

#include <vector>
#include <iostream>
#include <functional>
#include "core/fanAssert.hpp"

namespace fan
{
    struct EcsComponent;
    class EcsWorld;

    //==================================================================================================================================================================================================
    // A signal is a callback system used for communication between objects ( ~similar to qt Signals & Slots )
    //==================================================================================================================================================================================================
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
        void Emmit( Args... _args );
        void Clear();
        int ConnectionsCount() const { return (int)mConnections.size(); }
        void Disconnect( const size_t _ID );
        int GetType() const { return TemplateType<Args...>::Type(); }

    private:
        std::vector<Connection> mConnections;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename... Args >
    template< typename _Object >
    void Signal<Args...>::Connect( void (_Object::* _method)( Args... ), _Object* _object )
    {
        static_assert( !std::is_base_of<EcsComponent, _Object>::value );

        Connection connection;
        connection.mID     = (size_t)_object ;
        connection.mLambda = [_method, _object]( Args... _args )
        {
            ( ( *_object ).*( _method ) )( _args... );
        };
        mConnections.push_back( connection );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename... Args >
    void Signal<Args...>::Emmit( Args... _args )
    {
        for( Connection connection : mConnections )
        {
            connection.mLambda( _args... );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename... Args >
    void Signal<Args...>::Clear()
    {
        mConnections.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
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