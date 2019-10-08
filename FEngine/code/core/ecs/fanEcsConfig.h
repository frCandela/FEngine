#pragma once

#include "fanGlobalIncludes.h"
#include "core/meta/fanTypeList.h"
#include "core/ecs/fanBitsetCreator.h"

namespace fan {

	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct IComponent {};

	//================================
	struct CTranform : IComponent {
		btVector3		position;
		btQuaternion	rotation;
	};
	//================================
	struct CMovement : IComponent {
		btVector3		speed;
	};
	//================================
	struct CColor : IComponent {
		fan::Color		color;
	};
	//================================
	//================================
	using Components = meta::TypeList<
		CTranform
		, CMovement
		, CColor
	>;

	//================================================================================================================================
	// Tags
	//================================================================================================================================
	struct ITag {};
	struct TAlly : ITag {};
	struct TEnnemy : ITag {};
	//================================
	//================================
	using Tags = meta::TypeList<
		 TAlly
		,TEnnemy
	>;

	//================================================================================================================================
	// Bitsets & masks
	//================================================================================================================================
	using CEntity = uint32_t; 
	using Bitset = Bitset2::bitset2< 32 >;
	static_assert( Components::count + Tags::count <= 32 );

	static constexpr uint32_t aliveBit = Components::count + Tags::count;

	using BitsetsComponents = BitsetCreator<Bitset, Components>;
	using BitsetsTags		= BitsetCreator<Bitset, Tags, Components::count >;

	//================================================================================================================================
	// Signature
	// Contains the component & tags typelists and their associated bitsets
	// 'bitset' combines component & tags in the same bitset
	//================================================================================================================================
	template< typename _type > struct IsTag { static constexpr bool value = std::is_base_of< ITag, _type >::value; };
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< IComponent, _type >::value; };
	template< typename _componentType > struct IndexOfComponent { static constexpr size_t value = meta::Find::List< _componentType, Components >::value;	};
	template< typename _tagType >		struct IndexOfTag {		  static constexpr size_t value = meta::Find::List< _tagType, Tags >::value + Components::count;	};

	template< typename... ComponentsAndTags > 
	class Signature { 
	public:
		using componentsTypes = typename meta::Filter< IsComponent, ComponentsAndTags... >::type;
		using tagsTypes		  = typename meta::Filter< IsTag,	    ComponentsAndTags... >::type;
		static constexpr Bitset componentsBitset = BitsetsComponents::GetList<componentsTypes>::value;
		static constexpr Bitset tagsBitset = BitsetsTags::GetList<tagsTypes>::value;
		static constexpr Bitset bitset = componentsBitset | tagsBitset;
	}; 
	// testing
	static_assert( std::is_same< Signature<CMovement, TAlly>::componentsTypes, meta::TypeList< CMovement>>::value );
	static_assert( std::is_same< Signature<CMovement, TAlly>::tagsTypes, meta::TypeList< TAlly>>::value );
	//================================	
	//================================	
	using SDynamic	 = Signature< CTranform, CMovement >;
	using SEnnemyShip = Signature< CTranform, CMovement, TEnnemy >;
	using SAllyShip	 = Signature< CTranform, CMovement, TAlly >;
	
	static constexpr Bitset tot1 = SAllyShip::componentsBitset;
	static constexpr Bitset tot2 = SAllyShip::tagsBitset;
	static constexpr Bitset tot3 = SAllyShip::bitset;



}