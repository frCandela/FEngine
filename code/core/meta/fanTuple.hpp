#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/meta/fanHelpers.hpp"
#include "core/meta/fanTypeList.hpp"

namespace meta {
	namespace impl {
		//================================================================================================================================
		// TupleElement
		//================================================================================================================================
		template< size_t _index, typename _type > 
		struct TupleElement {
		protected:
			_type value;
		};

		//================================================================================================================================
		// TupleImpl
		//================================================================================================================================
		template< typename..._types >	struct TupleImpl;
	
		template< size_t... _Num, typename... _types >
		struct TupleImpl < typename SizeList<_Num...>, _types... >
			: TupleElement<_Num, _types>...
		{
		protected:
			template <size_t _index> 
			using ReturnType = typename Extract::Type<_index, _types...>::value;
			template < typename _type >	using indexElement = typename meta::Find::Type< _type,  _types... >;
		public:
			static constexpr size_t size = sizeof...( _types );

			template <size_t _index>
			ReturnType<_index>& Get() {
				return TupleElement<_index, ReturnType<_index> >::value;
			}

			template < typename _ComponentType >
			_ComponentType& Get()
			{
				return TupleElement<indexElement<_ComponentType>::value, _ComponentType >::value;
			}
		};
	}

	//================================================================================================================================
	// Group of variables with different types
	// Generates an index list to differentiate each type of the tuple and allow duplicating types
	// Ex:			Tuple< int, float, char >
	// Generates:	Tuple : TupleImpl< SizeList<0,1,2>, int, float, char >
 	//================================================================================================================================
	template< typename..._types > class Tuple;

	// Tuple with a raw template argument
	template< typename..._types > 
	class Tuple : public impl::TupleImpl< typename Range<sizeof...( _types )>::type, _types... >	{};

	// Tuple with a TypeList argument
	template< template < typename... > typename TypeList, typename... _types >
	class Tuple< TypeList<_types...>  > : public impl::TupleImpl< typename Range<sizeof...( _types )>::type, _types... > {};
}


	
	
	
	