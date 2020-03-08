#pragma once

#include "scene/fanScenePrecompiled.hpp"

namespace fan
{
	class ComponentsCollection;
	class EntityWorld;
	struct Entity;
	struct ecComponent;

	static constexpr uint32_t signatureLength = 32;
	using Signature = std::bitset<signatureLength>;
	static constexpr uint32_t ecAliveBit = signatureLength - 1;
	using EntityHandle = uint64_t;
	using EntityID = uint32_t;
	using ComponentIndex = uint8_t;
	using SingletonComponentID = int;
	using ChunckIndex = uint8_t;
	using ChunckComponentIndex = uint16_t;
}