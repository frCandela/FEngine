#pragma once

namespace fan {
	static const uint32_t s_maximumNumModels = 128;
	static const uint32_t s_maximumNumPointLights = 16;
	static const uint32_t s_maximumNumDirectionalLights = 4;

	struct DirectionalLightUniform {
		alignas( 16 ) glm::vec4 direction;
		alignas( 16 ) glm::vec4 ambiant;
		alignas( 16 ) glm::vec4 diffuse;
		alignas( 16 ) glm::vec4 specular;
	};

	struct PointLightUniform {
		alignas( 16 ) glm::vec4		position;
		alignas( 16 ) glm::vec4		diffuse;
		alignas( 16 ) glm::vec4		specular;
		alignas( 16 ) glm::vec4		ambiant;
		alignas( 4 ) glm::float32	constant;
		alignas( 4 ) glm::float32	linear;
		alignas( 4 ) glm::float32	quadratic;
		alignas( 4 ) glm::float32	_0;
	};

	struct LightsUniforms {
		DirectionalLightUniform dirLights[s_maximumNumDirectionalLights];
		PointLightUniform		pointlights[s_maximumNumPointLights];
		uint32_t   dirLightsNum;
		uint32_t   pointLightNum;
	};
}