#pragma once

#include <cstdint>

namespace fan
{
	class EcsArchetype;

	//================================
	//================================
	using EcsHandle = uint32_t;

	//================================
	// Allows indexing of individual entities
	//================================
	struct EcsEntity
	{
		EcsArchetype* archetype = nullptr;
		uint32_t	index = 0;	// index in the archetype
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