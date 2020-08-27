#pragma once

#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsArchetype;

	//========================================================================================================
	// Allows indexing of individual entities
	//========================================================================================================
	struct EcsEntity
	{
		EcsArchetype*   mArchetype = nullptr;
		uint32_t        mIndex = 0;	// index in the archetype

		bool operator!=( const EcsEntity& _other ) const
		{
			return mArchetype == _other.mArchetype && mIndex == _other.mIndex;
		}
	};

	//========================================================================================================
	// Data relative to a specific entity
	//========================================================================================================
	struct EcsEntityData
	{
		int       mTransitionIndex = -1;	// index of the corresponding transition if it exists
		EcsHandle mHandle          = 0;
	};
}