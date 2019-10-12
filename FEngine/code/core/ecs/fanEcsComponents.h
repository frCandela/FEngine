#pragma once

#include "core/meta/fanTypeList.h"
#include "core/math/shapes/fanAABB.h"

namespace fan {

	class Mesh;

	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct ecsIComponent {};

	//================================
	struct ecsTranform : ecsIComponent {
		static const char *  s_name;
		btVector3		position = btVector3::Zero();
		btQuaternion	rotation = btQuaternion::getIdentity();
	};

	//================================
	struct ecsScaling : ecsIComponent {
		static const char *  s_name;
		btVector3		scale = btVector3::One();
	};

	//================================
	struct ecsMovement : ecsIComponent {
		static const char *  s_name;
		btVector3		speed = btVector3::Zero();
	};

	//================================
	struct ecsParticle : ecsIComponent {
		static const char * s_name;
		fan::Color	color = Color::Red;
		float		durationLeft = 3.f;
	};

	//================================
	struct ecsAABB : ecsIComponent {
		static const char * s_name;
		AABB aabb;
	};

	//================================
	struct ecsModel : ecsIComponent {
		static const char * s_name;
		Mesh * m_mesh = nullptr;
		int m_renderID = -1;
	};

	//================================
	//================================
	using ecsComponents = meta::TypeList<
		ecsTranform
		, ecsMovement
		, ecsParticle
		, ecsScaling
		, ecsAABB
		, ecsModel
	>;
	 
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< ecsIComponent, _type >::value; };
}