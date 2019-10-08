#pragma once

#include "fanGlobalIncludes.h"
#include "core/meta/fanTypeList.h"
#include "core/ecs/fanBitsetCreator.h"

namespace fan {

	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct ecsIComponent {};

	//================================
	struct ecsTranform : ecsIComponent {
		btVector3		position;
		btQuaternion	rotation;
	};
	//================================
	struct ecsMovement : ecsIComponent {
		btVector3		speed;
	};
	//================================
	struct ecsColor : ecsIComponent {
		fan::Color		color;
	};
	//================================
	//================================
	using ecsComponents = meta::TypeList<
		ecsTranform
		, ecsMovement
		, ecsColor
	>;

	//================================================================================================================================
	// Tags
	//================================================================================================================================
	struct ecsITag {};
	struct ecsAlly : ecsITag {};
	struct ecsEnnemy : ecsITag {};
	//================================
	//================================
	using ecsTags = meta::TypeList<
		 ecsAlly
		,ecsEnnemy
	>;

	//================================================================================================================================
	// Bitsets & masks
	//================================================================================================================================
	using ecsEntity = uint32_t; 
	using ecsHandle = uint32_t;
	using ecsBitset = Bitset2::bitset2< 32 >;
	static_assert( ecsComponents::count + ecsTags::count <= 32 );

	static constexpr uint32_t aliveBit = ecsComponents::count + ecsTags::count;

	using ecsBitsetsComponents = BitsetCreator<ecsBitset, ecsComponents>;
	using ecsBitsetsTags		= BitsetCreator<ecsBitset, ecsTags, ecsComponents::count >;

	//================================================================================================================================
	// Signature
	// Contains the component & tags typelists and their associated bitsets
	// 'bitset' combines component & tags in the same bitset
	//================================================================================================================================
	template< typename _type > struct IsTag { static constexpr bool value = std::is_base_of< ecsITag, _type >::value; };
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< ecsIComponent, _type >::value; };
	template< typename _componentType > struct IndexOfComponent { static constexpr size_t value = meta::Find::List< _componentType, ecsComponents >::value;	};
	template< typename _tagType >		struct IndexOfTag {		  static constexpr size_t value = meta::Find::List< _tagType, ecsTags >::value + ecsComponents::count;	};

	template< typename... ComponentsAndTags > 
	class ecsSignature { 
	public:
		using componentsTypes = typename meta::Filter< IsComponent, ComponentsAndTags... >::type;
		using tagsTypes		  = typename meta::Filter< IsTag,	    ComponentsAndTags... >::type;
		static constexpr ecsBitset componentsBitset = ecsBitsetsComponents::GetList<componentsTypes>::value;
		static constexpr ecsBitset tagsBitset = ecsBitsetsTags::GetList<tagsTypes>::value;
		static constexpr ecsBitset bitset = componentsBitset | tagsBitset;
	}; 
	// testing
	static_assert( std::is_same< ecsSignature<ecsMovement, ecsAlly>::componentsTypes, meta::TypeList< ecsMovement>>::value );
	static_assert( std::is_same< ecsSignature<ecsMovement, ecsAlly>::tagsTypes, meta::TypeList< ecsAlly>>::value );
	//================================	
	//================================	
	using ecsDynamic	 = ecsSignature< ecsTranform, ecsMovement >;
	using ecsEnnemyShip = ecsSignature< ecsTranform, ecsMovement, ecsEnnemy >;
	using ecsAllyShip	 = ecsSignature< ecsTranform, ecsMovement, ecsAlly >;
	
	static constexpr ecsBitset tot1 = ecsAllyShip::componentsBitset;
	static constexpr ecsBitset tot2 = ecsAllyShip::tagsBitset;
	static constexpr ecsBitset tot3 = ecsAllyShip::bitset;



}