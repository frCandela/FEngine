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
	struct ecsScaling : ecsIComponent {
		btVector3		scale;
	};
	//================================
	struct ecsMovement : ecsIComponent {
		btVector3		speed;
	};
	//================================
	struct ecsParticle : ecsIComponent {
		fan::Color	color;
		float		durationLeft;
	};
	//================================
	//================================
	using ecsComponents = meta::TypeList<
		ecsTranform
		, ecsMovement
		, ecsParticle
		, ecsScaling
	>;

	//================================================================================================================================
	// Tags
	//================================================================================================================================
	struct ecsITag {};
	struct ecsFakeTag : ecsITag {};
	
	//================================
	//================================
	using ecsTags = meta::TypeList<
		ecsFakeTag
	>;

	//================================================================================================================================
	// Bitsets & masks
	//================================================================================================================================
	using ecsEntity = uint32_t; 
	using ecsHandle = uint64_t;
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
	static_assert( std::is_same< ecsSignature<ecsFakeTag, ecsMovement>::componentsTypes, meta::TypeList< ecsMovement>>::value );
	static_assert( std::is_same< ecsSignature<ecsMovement, ecsFakeTag>::tagsTypes, meta::TypeList< ecsFakeTag>>::value );
	
	//================================	
	// Declare your signatures here
	//================================	
	using ecsParticleSignature = ecsSignature< ecsTranform, ecsMovement, ecsParticle >;
	
	static constexpr ecsBitset tot1 = ecsParticleSignature::componentsBitset;

	//================================================================================================================================
	// System
	// Runs logic on entities matching a specific signature
	//================================================================================================================================
	template < typename... _signature> class ISystem;
	template < template <typename...> typename _typeList, typename... _args>
	class ISystem <_typeList<_args...> >{
		public:	
			using signature = ecsSignature<_args...>;
			virtual void Run( _args&... ) = 0;
	};

	//================================
	// Declare your systems here
	//================================
	class MoveParticle : ISystem<  ecsParticleSignature > {
		void Run( ecsTranform& /*_transform*/, ecsMovement& /*_movement*/, ecsParticle& /*_particle*/ ) override {

		}
	};

	//using caca = MoveParticle::signature::componentsTypes;
}