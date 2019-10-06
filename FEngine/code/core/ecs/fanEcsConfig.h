#pragma once

#include "core/meta/fanTypeList.h"
#include "fanGlobalIncludes.h"

namespace ecs {
	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct IComponent {};

	//================================
	struct Tranform : IComponent {
		btVector3		position;
		btQuaternion	rotation;
	};
	//================================
	struct Movement : IComponent {
		btVector3		speed;
	};
	//================================
	struct Color : IComponent {
		fan::Color		color;
	};
	//================================
	//================================
	using Components = meta::TypeList<
		Tranform
		, Movement
		, Color
	>;

	//================================================================================================================================
	// Tags
	//================================================================================================================================
	struct ITag {};
	struct Ally : ITag {};
	struct Ennemy : ITag {};
	//================================
	//================================
	using Tags = meta::TypeList<
		 Ally
		,Ennemy
	>;

	//================================================================================================================================
	// Bitsets & masks
	//================================================================================================================================
	using Entity = uint32_t; 
	using Bitset = Bitset2::bitset2< 32 >;
	static_assert( Components::count + Tags::count  <= 32 );
	#include "core/ecs/fanBitsetCreator.h"
	
	using ComponentsBitsets = BitsetCreator<Bitset, Components>;
	using TagsBitsets		= BitsetCreator<Bitset, Tags>;

	//================================================================================================================================
	// Signature
	// Contains the component & tags typelists and their associated bitsets
	// 'bitset' combines component & tags in the same bitset
	//================================================================================================================================
	template< typename _type > struct IsTag { static constexpr bool value = std::is_base_of< ITag, _type >::value; };
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< IComponent, _type >::value; };
	template< typename... ComponentsAndTags > 
	class Signature {
	public:
		using componentsTypes = typename meta::Filter< IsComponent, ComponentsAndTags... >::type;
		using tagsTypes		  = typename meta::Filter< IsTag,	    ComponentsAndTags... >::type;
		static constexpr Bitset componentsBitset = ComponentsBitsets::GetList<componentsTypes>::value;
		static constexpr Bitset tagsBitset = TagsBitsets::GetList<tagsTypes>::value;
		static constexpr Bitset bitset = componentsBitset | (tagsBitset << Components::count);
	};
	// testing
	static_assert( std::is_same< Signature<Movement, Ally>::componentsTypes, meta::TypeList< Movement>>::value );
	static_assert( std::is_same< Signature<Movement, Ally>::tagsTypes, meta::TypeList< Ally>>::value );
	//================================	
	//================================	
	using Dynamic	 = Signature< Tranform, Movement >;
	using EnnemyShip = Signature< Tranform, Movement, Ennemy >;
	using AllyShip	 = Signature< Tranform, Movement, Ally >;
	
	static constexpr Bitset tot1 = AllyShip::componentsBitset;
	static constexpr Bitset tot2 = AllyShip::tagsBitset;
	static constexpr Bitset tot3 = AllyShip::bitset;

}