#pragma once

namespace fan
{
	//========================================================================================================
	// static singleton class
	// 99% of the time you don't need this
	// Better use ecs singletons in the game/editor ecs
	//========================================================================================================
	template < typename TypeName >
	class Singleton
	{
	public:
		static TypeName& Get()
		{
			static TypeName instance;
			return instance;
		};

	protected:
		Singleton() {}
		Singleton< TypeName>& operator= ( const Singleton< TypeName>& ) = delete;
		Singleton< TypeName>( const Singleton< TypeName>& ) = delete;
	};
}