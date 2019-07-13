#pragma once

#include "fanIncludes.h"

namespace vk {

	inline std::array< btVector3, 36 > GetCube(const float _halfSize);
	inline std::vector<btVector3> GetSphere(const float _radius, const int _numSubdivisions);
	inline std::vector<btVector3> GetCone(const float _radius, const float _height, const int _numSubdivisions);
}

#include "vulkan/util/vkShape.inl"