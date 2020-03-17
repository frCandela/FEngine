#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	// MotionState is used synchronizes physics with graphics
	//==============================================================================================================================================================
	struct MotionState : public Component
	{
		DECLARE_COMPONENT( MotionState )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void Save( const Component& _component, Json& _json ) {}
		static void Load( Component& _component, const Json& _json ) {}

		btDefaultMotionState motionState;
	};
	static constexpr size_t sizeof_motionState = sizeof( MotionState );
}