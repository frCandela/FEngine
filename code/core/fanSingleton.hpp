#pragma once
 
#include "core/fanCorePrecompiled.hpp"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	template < typename TypeName > 
	class Singleton {
	public:		
		static TypeName& Get() { 
			static TypeName instance;
			return instance;
		};

	protected:
		Singleton() {}
		Singleton< TypeName>& operator= (const Singleton< TypeName>&) = delete;
		Singleton< TypeName>(const Singleton< TypeName>&) = delete;
	};
}