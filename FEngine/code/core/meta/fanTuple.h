#pragma once

#include "core/meta/fanHelpers.h"

namespace meta {
	//================================================================================================================================
	// TupleElement
	//================================================================================================================================
	template< size_t _Index, typename _Type > struct TupleElement
	{
		_Type value;
	};

	//================================================================================================================================
	// TupleImpl
	// Ex:			Tuple< int, float >
	// Generates:	TupleImpl : TupleElement<0,int>, TupleElement<1,float>
	//================================================================================================================================
	template< typename..._Types >	struct TupleImpl;
	
	template< size_t... _Num, typename... _Types >
	struct TupleImpl < typename SizeList<_Num...>, _Types... >
		: TupleElement<_Num, _Types>...
	{
	private:
		template <size_t _Index> 
		using ReturnType = typename Choose<_Index, _Types...>::type;

	public:
		template <size_t _Index>
		ReturnType<_Index>& Get() { 
			return TupleElement<_Index, ReturnType<_Index> >::value; 
		}
	};


	//================================================================================================================================
	// Tuple
	// List of variables with different types
	// Generates an index list to differentiate each type of the tuple and allow duplicating types
	// Ex:			Tuple< int, float, char >
	// Generates:	Tuple : TupleImpl< SizeList<0,1,2>, int, float, char >
	//================================================================================================================================
	template< typename..._Types >
	struct Tuple
		: TupleImpl< typename Range<sizeof...( _Types )>::type, _Types... >
	{
		static constexpr size_t size  = sizeof...( _Types );
	};

}


	
	
	
	