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
		btVector3		position = btVector3::Zero();
		btQuaternion	rotation = btQuaternion::getIdentity();
	};
	//================================
	struct ecsScaling : ecsIComponent {
		btVector3		scale = btVector3::One();
	};
	//================================
	struct ecsMovement : ecsIComponent {
		btVector3		speed = btVector3::Zero();
	};
	//================================
	struct ecsParticle : ecsIComponent {
		fan::Color	color			= Color::Red;
		float		durationLeft	= 3.f;
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
	const ecsEntity ecsNullEntity = std::numeric_limits< ecsEntity >::max();
	using ecsHandle = uint64_t;
	const ecsHandle ecsNullHandle = std::numeric_limits< ecsHandle >::max();
	using ecsBitset = Bitset2::bitset2< 32 >;
	static_assert( ecsComponents::count + ecsTags::count <= 32 );

	static constexpr uint32_t aliveBit = ecsComponents::count + ecsTags::count;

	using ecsBitsetsComponents = BitsetCreator<ecsBitset, ecsComponents>;
	using ecsBitsetsTags		= BitsetCreator<ecsBitset, ecsTags, ecsComponents::count >;
}