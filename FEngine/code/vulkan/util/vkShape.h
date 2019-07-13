#pragma once

namespace vk {
	inline std::array< btVector3, 36 > GetCube(const float _halfSize) {
		const btVector3 v0(+_halfSize, +_halfSize, +_halfSize);
		const btVector3 v1(+_halfSize, +_halfSize, -_halfSize);
		const btVector3 v2(-_halfSize, +_halfSize, +_halfSize);
		const btVector3 v3(-_halfSize, +_halfSize, -_halfSize);
		const btVector3 v4(+_halfSize, -_halfSize, +_halfSize);
		const btVector3 v5(+_halfSize, -_halfSize, -_halfSize);
		const btVector3 v6(-_halfSize, -_halfSize, +_halfSize);
		const btVector3 v7(-_halfSize, -_halfSize, -_halfSize);

		return {
			 v0,v1,v2	,v1,v3,v2	// top
			,v6,v5,v4	,v7,v5,v6	// bot
			,v7,v6,v2	,v7,v2,v3
			,v6,v4,v0	,v6,v0,v2
			,v4,v5,v0	,v5,v1,v0
			,v7,v1,v5	,v7,v3,v1
		};
	}

}