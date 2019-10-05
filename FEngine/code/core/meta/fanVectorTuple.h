#pragma once

#include "fanHelpers.h"
#include "fanTypeList.h"

#include <bitset>

namespace meta {
	namespace impl {
		//================================================================================================================================
		// VectorData 
		// Contains a vector of _type and it's associated bitset
		//================================================================================================================================
		template < typename _type, size_t _index, size_t _count >
		struct	VectorData {
			std::vector<_type>		vector;
			static constexpr std::bitset<_count> mask = std::bitset < _count>(1 << _index);
		};

		//================================================================================================================================
		// VectorElement
		// Defines data for a specific _type at an _index
		// _count is the number of VectorElements of the VectorTuple
		//================================================================================================================================
		template < typename _type, size_t _index, size_t _count >
		struct	VectorElement {
			VectorData<_type, _index, _count > data;
		};

		//================================================================================================================================
		// VectorTupleImpl
		// Inherits multiples VectorElement of differents types
		// Get() gives access to differents elements of the VectorTuple
		//================================================================================================================================
		template < typename... _types >	struct VectorTupleImpl;	
		template < template< size_t...> typename _SizeList, size_t... _sizes, typename... _types  >
		struct VectorTupleImpl< _SizeList<_sizes...>, _types... > : VectorElement<_types, _sizes, sizeof...( _types )>...
		{		
		private:
			// returns the index of the corresponding _type
			template < typename _type >	using indexElement = typename Find::Type< _type,  _types... >;

		public:
			// Returns the vector data of the corresponding _type
			template < typename _type >
			VectorData< _type, indexElement<_type>::value, sizeof...( _types )  >& Get() {
 				return  VectorElement< _type, indexElement<_type>::value, sizeof...( _types )  >::data;
 			}
		};
	}

	//================================================================================================================================
	// Vector Tuple
	// Generates a vectors & bitset for every type of _types... 
	//================================================================================================================================
	template< typename..._types > class VectorTuple;
	
	// Tuple with a TypeList argument
	template< template < typename... > typename TypeList, typename... _types >
	class VectorTuple<TypeList<_types...> >  : public impl::VectorTupleImpl < typename Range< sizeof...( _types ) >::type, _types... >
	{
	public:
		static constexpr size_t size = sizeof...( _types );		// Number of types in the tuple
	};

}

