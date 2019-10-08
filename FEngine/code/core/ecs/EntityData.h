#pragma once

#include "core/ecs/fanECSConfig.h"

namespace fan {
	struct EntityData {
	public:
		EntityData() {
			bitset[aliveBit] = 1;
		}
		uint32_t components[Components::count];
		Bitset   bitset; // signature bitset

		void Kill() { bitset[aliveBit] = 0; }
		bool IsAlive() const { return   bitset[aliveBit]; }
		bool IsDead() const { return  ! bitset[aliveBit]; }
	};
	static_assert( sizeof(EntityData) == sizeof( Bitset ) + Components::count * sizeof( uint32_t ) ); // Beware of paaaaaaading :')
}