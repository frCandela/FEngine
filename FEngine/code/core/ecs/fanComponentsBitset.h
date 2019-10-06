#pragma once

#include "core/meta/fanTypeList.h"
#include "core/meta/fanHelpers.h"


namespace meta {
	//================================================================================================================================
	// Computes the bitmask of one or multiple  components
	//================================================================================================================================
	template < typename _bitsetType, typename _mainComponentsList, typename... _components > class ComponentsBitSetImpl;

	template <typename _bitsetType, template < typename... > typename _mainComponentsList, typename... _types, typename... _components>
	class ComponentsBitSetImpl<_bitsetType, _mainComponentsList<_types...>, _components... > {
	private:
		// bitmask of one component
		template < typename _component >
		static constexpr _bitsetType componentBitset = _bitsetType( 1 ) << meta::Find::List<_component, _mainComponentsList>::value;

		// GetBitsetImpl declaration
		template < typename... _components > struct GetBitsetImpl;

		// Specialization 
		template < typename _tail>
		struct GetBitsetImpl<_tail> {
			static constexpr _bitsetType value = componentBitset<_tail>;
		};

		// General case
		template < typename _head, typename... _tail >
		struct GetBitsetImpl<_head, _tail...> {
			static constexpr _bitsetType value = componentBitset<_head> | GetBitsetImpl<_tail...>::value;
		};
	public:
		static constexpr _bitsetType value = GetBitsetImpl<_components...>::value;
	};
}