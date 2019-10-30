#pragma once

#include "ecs/fanECSConfig.h"

namespace fan {
	struct ecsComponentsKey 
	{
	public:
		ecsComponentsKey() {	bitset[aliveBit] = 1; }

		uint16_t	chunck[ecsComponents::count];		// chunck of each components
		uint16_t	element[ecsComponents::count];	// index of each components
		ecsBitset   bitset;								// signature

		void Kill() { bitset[aliveBit] = 0; }
		bool IsAlive() const { return   bitset[aliveBit]; }
		bool IsDead() const { return  ! bitset[aliveBit]; }
	};
}