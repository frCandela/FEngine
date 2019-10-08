#pragma once

#include "core/meta/fanTypeList.h"
#include "core/meta/fanHelpers.h"

//================================================================================================================================
// BitsetCreator : Computes the bitmask of one or multiple  types
// _referenceTypes is the list of types compared against
// Get functions return the bitset corresponding to the given types
//================================================================================================================================
template < typename _bitSetType, typename _referenceTypes, size_t _offset = 0 >
class BitsetCreator {
public:
	// bitmask of one type
	template < typename _singleType >
	static constexpr _bitSetType singleTypeBitset = _bitSetType( 1 ) << (_offset + meta::Find::List<_singleType, _referenceTypes>::value );

public:
	// GetBitsetImpl declaration
	template < typename... T > struct Get;

	// Specialization with one element
	template < typename _tail>
	struct Get<_tail> {
		static constexpr _bitSetType value = singleTypeBitset<_tail>;
	};

	// Specialization with 0 element
	template <>
	struct Get<> {
		static constexpr _bitSetType value = _bitSetType(0);
	};

	// General case with raw types...
	template < typename _head, typename... _tail >
	struct Get<_head, _tail...> {
		static constexpr _bitSetType value = ( singleTypeBitset<_head> | Get<_tail...>::value );
	};

	// General case with a typelist
	template < typename _typesList > struct GetList;
	template < template < typename... > typename _typesList, typename... _types >
	struct GetList<_typesList<_types...>> {
		static constexpr _bitSetType value = Get<_types...>::value;
	};
};

// Testing
namespace impl {
	class A{}; class B {}; class C {};
	using TestBitsetType = size_t;
	using TestBitset = BitsetCreator< TestBitsetType, meta::TypeList<A,B,C> >;
	static_assert( TestBitset::Get<A>::value == TestBitsetType(1) );
	static_assert( TestBitset::Get<C>::value == TestBitsetType(4) );
	static_assert( TestBitset::Get<A,C>::value == TestBitsetType( 0b101 ) );
}
