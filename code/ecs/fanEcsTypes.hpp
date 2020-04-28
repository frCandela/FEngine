#pragma once

#include <stdint.h>
#include <bitset>
#include <vector>

namespace fan
{
	//==============================================================================================================================================================
	// base types & sizes for the ecs
	//==============================================================================================================================================================
	class ComponentsCollection;
	class EcsWorld;
	struct Entity;
	struct Component;

	static constexpr uint32_t signatureLength = 64;
	using Signature = std::bitset<signatureLength>;
	static constexpr uint32_t ecAliveBit = signatureLength - 1;
	using EntityHandle = uint64_t;
	using EntityID = uint32_t;
	using ComponentIndex = uint8_t;
	using SingletonComponentID = int;
	using ChunckIndex = uint8_t;
	using ChunckComponentIndex = uint16_t;
	using FrameIndex = uint32_t;
}