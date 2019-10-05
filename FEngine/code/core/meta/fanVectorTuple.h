#pragma once

#include "fanHelpers.h"
#include "fanTypeList.h"

#include <bitset>

namespace meta {
	//================================================================================================================================
	// Array Element
	//================================================================================================================================
// 	template < typename _Type, size_t _Index >
// 	struct	VectorData {
// 		std::vector<_Type>		vector;
// 		static constexpr size_t index = 3;
// 	};

	//================================================================================================================================
	// VectorTupleImpl
	//================================================================================================================================
// 	template < typename _Type >
// 	struct VectorElement
// 	{
// 		VectorData <_Type, 0 > data;
// 	};

	//================================================================================================================================
	// Array Tuple
	// No duplicates types are allowed
	//================================================================================================================================
// 	template < typename... _Types >
// 	struct	VectorTuple  : VectorElement < _Types >...
// 	{
// 	private:
// 		template < typename _Type >	using index = Find< _Type, _Types...>::index;
// 
// 	public:
// 		// Number of types in the tuple
// 		static constexpr size_t size = sizeof...( _Types );
// 
// 		// Returns the VectorData of a _Type
// 		template < typename _Type >
// 		VectorData< _Type, index<_Type>::index  >& Get() {
// 			return  VectorElement< _Type >::data;
// 		}
// 	};

}

