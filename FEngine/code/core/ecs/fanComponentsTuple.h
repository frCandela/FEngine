#pragma once

#include "core/meta/fanHelpers.h"
#include "core/meta/fanTypeList.h"

namespace fan {
	namespace impl {
		//================================================================================================================================
		// ComponentElement
		// Defines data for a specific _type at an _index
		// _count is the number of ComponentElement of the ComponentsTuple
		//================================================================================================================================
		template < size_t _index, typename _type >
		struct	ComponentElement {
			std::vector<_type>	vector;
		};

		//================================================================================================================================
		// ComponentsTupleImpl
		// Inherits multiples ComponentElement of differents types
		// Get() gives access to differents elements of the ComponentsTuple
		//================================================================================================================================
		template < typename... _types >	struct ComponentsTupleImpl;	
		template < template< size_t...> typename _SizeList, size_t... _sizes, typename... _types  >
		struct ComponentsTupleImpl< _SizeList<_sizes...>, _types... > : ComponentElement<_sizes, _types >...
		{		
		private:
			// returns the index of the corresponding _type
			template < typename _type >	using indexElement = typename meta::Find::Type< _type,  _types... >;

		public:
			// Returns the ComponentData of the corresponding _type
			template < typename _type >
			std::vector< _type>& Get() {
 				return  ComponentElement< indexElement<_type>::value, _type >::vector;
 			}
		};
	}

	//================================================================================================================================
	// ComponentsTuple
	// Generates a vectors & bitset for every type passed in _types
	//================================================================================================================================
	template< typename..._types > class ComponentsTuple;
	
	// Tuple with a TypeList argument
	template< template < typename... > typename TypeList, typename... _types >
	class ComponentsTuple<TypeList<_types...> >  : public impl::ComponentsTupleImpl < typename meta::Range< sizeof...( _types ) >::type, _types... >
	{
	public:
		static constexpr size_t size = sizeof...( _types );		// Number of types in the tuple
	};
}

