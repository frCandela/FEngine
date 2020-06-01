#pragma once

#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsArchetype;

	//================================
	// Allows indexing of individual entities
	//================================
	struct EcsEntity
	{
		EcsArchetype* archetype = nullptr;
		uint32_t	index = 0;	// index in the archetype

		bool operator!=( const EcsEntity& _other ) const
		{
			return archetype == _other.archetype && index == _other.index;
		}
	};

	//================================
	// Data relative to a specific entity
	//================================
	struct EcsEntityData
	{
		int	 transitionIndex = -1;	// index of the corresponding transition if it exists
		EcsHandle handle = 0;
	};
}