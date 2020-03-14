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
		static void Init( ecComponent& _component );
		static void Save( const ecComponent& _component, Json& _json ) {}
		static void Load( ecComponent& _component, const Json& _json ) {}

		btDefaultMotionState motionState;
	};
	static constexpr size_t sizeof_motionState = sizeof( MotionState );
}