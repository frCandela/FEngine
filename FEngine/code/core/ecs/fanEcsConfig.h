#pragma once

#include "core/ecs/fanEcsComponents.h"
#include "core/ecs/fanEcsComponentsTuple.h"
#include "core/ecs/fanEcsBitsetCreator.h"

namespace fan {

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
	const ecsEntity ecsNullEntity = std::numeric_limits< ecsEntity >::max();
	using ecsHandle = uint64_t;
	const ecsHandle ecsNullHandle = std::numeric_limits< ecsHandle >::max();
	using ecsBitset = Bitset2::bitset2< 32 >;
	static_assert( ecsComponents::count + ecsTags::count <= 32 );

	static constexpr uint32_t aliveBit = ecsComponents::count + ecsTags::count;

	using ecsBitsetsComponents = BitsetCreator<ecsBitset, ecsComponents>;
	using ecsBitsetsTags		= BitsetCreator<ecsBitset, ecsTags, ecsComponents::count >;
}