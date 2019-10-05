#pragma once

#include "core/meta/fanHelpers.h"

namespace meta {

	//================================================================================================================================
	// List of types
	//================================================================================================================================
	template <typename... Types>
	class TypeList {};

	//================================================================================================================================
	// Extract N-th type in a list of types
	// Example:
	//	using types = TypeList<std::string, int, char, float>;
	//	Extract::List<1, types>::value
	//	Extract::Type<1, std::string, int, char, float>::value
	//================================================================================================================================	
	class Extract {
	private:
		template<size_t _index, typename ... _list > struct Choose;

		// Specialization 
		template<typename _head, typename ... _list>
		struct Choose<0, _head, _list... > : Id<_head> {};

		// General case
		template<size_t _index, typename _Head, typename ... _list>
		struct Choose<_index, _Head, _list...> : Choose<_index - 1, _list...> {};

	public:		
		// Extract on a raw template 
		template< size_t _index, typename... _types > struct Type {
			using value = typename Choose<_index, _types...>::type;
		};

		// Extract on a TypeList
		template< size_t _index, typename _typeList > struct List;
		template< size_t _index, template < typename... > typename TypeList, typename... _types >
		struct List < _index, TypeList<_types...>  > {
			using value = typename Choose<_index, _types...>::type;
		};
	};

	//================================================================================================================================
	// Finds the index of the first _Type element in a _List
	// Example:
	//	using types = meta::TypeList<std::string, int, char, float>;
	//	Extract::List<1, types>::value var = Find::Type<char, std::string, int, char, float >::value;
	//	Extract::List<1, types>::value var2 = Find::List< float, types >::value;
	//================================================================================================================================
	class Find {		
		template<  typename _type, size_t _Index, typename _head, typename ... _tail > struct FindImpl;

		// Specialization 
		template<  typename _type, size_t _Index, typename ... _tail >
		struct FindImpl<_type, _Index, _type, _tail... > {
			static constexpr size_t Get() { return _Index;	}
		};

		// General case
		template<  typename _type, size_t _Index, typename _head, typename ... _tail >
		struct FindImpl : FindImpl<_type, _Index + 1, _tail... > {};

	public:
		// Find on a raw template 
		template< typename _element, typename... _types > struct Type {
			static constexpr size_t value = typename FindImpl<_element, 0, _types...>::Get();
		};

		// Find on a TypeList
		template< typename _element, typename _typeList > struct List;
		template< typename _element, template < typename... > typename TypeList, typename... _types >
		struct List < _element, TypeList<_types...>  > {
			static constexpr size_t value = typename FindImpl<_element, 0, _types...>::Get();
		};
	};
}