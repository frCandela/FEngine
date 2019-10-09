#pragma once

#include "fanGlobalIncludes.h"
#include "core/ecs/fanComponentsTuple.h"
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
}