#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// MotionState is used synchronizes physics transforms with graphics transforms
	//==============================================================================================================================================================
	struct MotionState : public Component
	{
		DECLARE_COMPONENT( MotionState )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void Save( const Component& _component, Json& _json ) {}
		static void Load( Component& _component, const Json& _json ) {}

		btDefaultMotionState motionState;
	};
	static constexpr size_t sizeof_motionState = sizeof( MotionState );
}