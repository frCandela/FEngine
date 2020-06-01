#pragma once

#include <vector>
#include <functional>
#include <cassert>
#include "ecs/fanEcsWorld.hpp"


namespace fan
{
	struct EcsComponent;
	class EcsWorld;

	//================================================================================================================================
	// A signal is used for communication between objects ( similar to qt Signals & Slots )
	//
	// ex : 
	// struct MyClass {void TestFunction( int ) {} {
	// MyClass test;
	// Signal<int> signal;
	// signal.Connect( &MyClass::TestFunction, &test );
	// signal.Emmit( 42 );
	//
	// Warning : The pointer to the connected object (_object) must stay valid during the whole lifetime of the signal
	// When connecting to EcsComponents, you must pass the EcsWorld and an EcsHandle of your component to the Connect method.
	//================================================================================================================================
	template < typename ..._Args > struct Signal
	{
		using Connection = std::function<void( _Args... )>; // lambda

		//================================================================
		//================================================================
		template <typename _Object >
		void Connect( void( _Object::* _method )( _Args... ), _Object* _object )
		{
			static_assert( ! std::is_base_of< EcsComponent, _Object>::value );

			Connection connection = [_method, _object]( _Args... _args ) { ( ( *_object ).*( _method ) )( _args... ); };
			connections.push_back( connection );
		}

		//================================================================
		//================================================================
		template <typename _ComponentType >
		void Connect( void( _ComponentType::* _method )( _Args... ), EcsWorld& _world,  const EcsHandle _handle )
		{
			static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
			assert( _handle != 0 );

			Connection connection = [_method, &_world, _handle]( _Args... _args )
			{ 
				_ComponentType& component = _world.GetComponent<_ComponentType>( _world.GetEntity( _handle ) );
				( ( component ).*( _method ) )( _args... );
			};
			connections.push_back( connection );
		}

		//================================================================
		//================================================================
		void Emmit( _Args... _args )
		{
			for( Connection connection : connections )
			{
				connection( _args... );
			}
		}

		//================================================================
		//================================================================
		void Clear() 
		{ 
			connections.clear(); 
		}

		std::vector< Connection > connections;
	};
}