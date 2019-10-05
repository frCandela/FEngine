#pragma once

#include "fanHelpers.h"
#include "fanTypeList.h"

namespace meta {
	//================================================================================================================================
	// Array Tuple
	//================================================================================================================================
	template < typename ... _List > struct ArrayTuple;
	
	template < typename _Head >
	struct	ArrayTuple< _Head > {
		std::array<_Head, 64> m_array;
	};
	
	 template < typename _Head, typename ..._Tail >
	 struct	ArrayTuple< _Head, _Tail... > : ArrayTuple < _Tail... > {
		 std::array<_Head, 64> m_array;
	};
}

