#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	// MotionState is used synchronizes physics with graphics
	//==============================================================================================================================================================
	struct MotionState : public ecComponent
	{
		DECLARE_COMPONENT( MotionState )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Clear( ecComponent& _motionState );

		btDefaultMotionState motionState;
	};
	static constexpr size_t sizeof_motionState = sizeof( MotionState );
}