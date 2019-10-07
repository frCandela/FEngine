#pragma once

#include "core/meta/fanHelpers.h"

namespace meta {

	//================================================================================================================================
	// List of types
	//================================================================================================================================
	template <typename... _types>
	class TypeList {
	public:
		static constexpr size_t count = sizeof...( _types );
	};

	//================================================================================================================================
	// Extract N-th type in a list of types
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

	// Testing
	namespace impl {
		using types = meta::TypeList<long, int, char, float>;
		static_assert( std::is_same < Extract::List<0, types>::value, long>::value );
		static_assert( std::is_same< Extract::List<2, types>::value, char >::value );
	}

	//================================================================================================================================
	// Finds the index of the first _Type element in a _List
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

	// Testing
	namespace impl {
		using types = meta::TypeList<long, int, char, float>;
		static_assert( Find::List<char, types>::value == 2 );
		static_assert( Find::List<long, types>::value == 0 );
	}

	//================================================================================================================================
	// class Concat
	// Merges a group of typelists into one
	//================================================================================================================================
	template< typename... _typeLists >
	class Concat {
	private:
		// Special concat for 2 elements
		template< typename _type1, typename _type2 > struct Concat2;
		template< template <typename...> typename TypeList, typename... _types1, typename... _types2 >
		struct Concat2<TypeList<_types1...>, TypeList<_types2...>> {
			using type = TypeList<_types1..., _types2...>;
		};

		// Decl
		template< typename... _types > struct ConcatImpl;

		// Specialization 
		template< typename _type1, typename _type2 >
		struct ConcatImpl<_type1, _type2 > {
			using type = typename Concat2<_type1, _type2>::type;
		};

		// General case
		template< typename _type1, typename _type2, typename... _rest >
		struct ConcatImpl<_type1, _type2, _rest... > {
			using type = typename ConcatImpl< typename Concat2<_type1, _type2 >::type, _rest...>::type;
		};

	public:
		using type = typename ConcatImpl<_typeLists...>::type;
	};

	// Testing
	namespace impl {
		using ConcatTest = typename Concat< TypeList<int, float>, TypeList<>, TypeList<char>>::type;
		using ConcatResult = TypeList<int, float, char>;
		static_assert( std::is_same<ConcatTest, ConcatResult>::value );
	}

	//================================================================================================================================
	// Creates a new typeList using the filter passed as an argument
	//================================================================================================================================
	template< template< typename> typename _predicate, typename... _types >
	class Filter {
	private:
		// General case (true)
		template <typename T, bool>
		struct FilterIfResult {
			using type = TypeList<T>;
		};
		// Specialization (false)
		template < typename T >
		struct FilterIfResult<T, false> {
			using type = TypeList<>;
		};

		// Helper
		template< typename _type, template< typename> typename _predicate >
		struct FilterImpl {
			using value = typename FilterIfResult< _type, _predicate<_type>::value >::type;
		};

	public:
		using type = typename Concat< typename FilterImpl< _types, _predicate>::value...>::type;
	};

	// Testing
	namespace impl {
		class A{};
		class B{};
		class C{};
		template< typename _type > struct IsClassA { static constexpr bool value = std::is_same< A, _type >::value; };
		using testFilter = meta::Filter< IsClassA, C, A, A, B,C, B, A, B, B>::type;
		using testFilterResult = meta::TypeList<A, A, A>;
		static_assert( std::is_same< testFilter, testFilterResult >::value );
	}
}
