#pragma once

namespace meta {
	//================================================================================================================================
	// Simple id
	//================================================================================================================================
	template< typename _type >
	class Id {
	public:
		using type = _type;
	};

	//================================================================================================================================
	// List of size_t args
	//================================================================================================================================
	template< size_t ... _args >
	class SizeList : public Id< SizeList< _args... > > {};

	//================================================================================================================================
	// Range
	// given L>=0, generate sequence <0, ..., L-1>
	//================================================================================================================================
	namespace impl {
		template< size_t _index, size_t... _range > struct RangeImpl;

		// Specialization 
		template< size_t... _range >
		struct RangeImpl< 0, _range...> : SizeList<_range...> {};

		// General case
		template< size_t _index, size_t... _range >
		struct RangeImpl : RangeImpl< _index -1, _index -1, _range...>{};
	}
	template< size_t _index> class Range : public impl::RangeImpl<_index - 1, _index - 1>{};

}