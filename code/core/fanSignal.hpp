#pragma once

#include <vector>
#include <functional>
#include <cassert>

namespace fan
{
	//================================================================================================================================
	// A signal is used for communication between objects ( similar to qt Signals & Slots )
	//
	// ex : 
	// struct MyClass {void TestFunction( int ) {} {
	// MyClass test;
	// Signal<int> signal;
	// signal.Connect( &MyClass::TestFunction, &test );
	// signal.Emmit( 42 );
	//================================================================================================================================
	template < typename ..._Args > struct Signal
	{
		//================================================================
		//================================================================
		struct Connection
		{
			void* object = nullptr;
			std::function<void( _Args... )> lambda;
		};

		template <typename _Object >
		void Connect( void( _Object::* _method )( _Args... ), _Object* _object )
		{
			std::function<void( _Args... )> func = [_method, _object]( _Args... _args ) { ( ( *_object ).*( _method ) )( _args... ); };
			Connection c = { _object, func };
			m_connections.push_back( c );
		}

		template <typename _Object >
		void Disconnect( _Object* _object )
		{
			for( int i = 0; i < m_connections.size(); i++ )
			{
				Connection& connection = m_connections[i];
				if( connection.object )
				{
					m_connections.erase( m_connections.begin() + i );
					return;
				}
			}
		}

		void Emmit( _Args... _args )
		{
			for( auto connection : m_connections )
			{
				connection.lambda( _args... );
			}
		}
		void Clear() { m_connections.clear(); }

		std::vector< Connection > m_connections;
	};
}