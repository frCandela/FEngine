#pragma once

#include <cstdint>
#include <bitset>

namespace fan
{
	static constexpr uint32_t ecsSignatureLength = 64;
	using EcsSignature = std::bitset<ecsSignatureLength>;
	using EcsHandle = uint32_t;
	using FrameIndex = uint32_t;
}