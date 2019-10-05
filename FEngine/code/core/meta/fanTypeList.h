#pragma once

#include "core/meta/fanHelpers.h"

namespace meta {
	//================================================================================================================================
	// Type List 
	//================================================================================================================================
	struct NullType;

	// General type list 
	template< typename _THead, typename ... _TTypes >
	struct TypeList {
		using head = _THead;
		using tail = typename TypeList< _TTypes... >;
	};

	// Type list specialization with a single element
	template< typename _THead >
	struct TypeList< _THead > {
		using head = _THead;
		using tail = NullType;
	};

	//================================================================================================================================
	// Type list extractor
	//================================================================================================================================
	template< size_t _Index, typename _TypeList  >
	struct Extract {
		using result = typename Extract<_Index - 1, typename _TypeList::tail >::result;
	};

	template< typename _TypeList  >
	struct Extract<0, _TypeList > {
		using result = typename _TypeList::head;
	};
}