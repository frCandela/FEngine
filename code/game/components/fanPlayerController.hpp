#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// PlayerController can be the mouse/keyboard or a game pad
	// @wip gamepad is disabled for now
	//================================================================================================================================
	struct PlayerController : public Component
	{
		DECLARE_COMPONENT( PlayerController )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

// 		enum InputType { KEYBOARD_MOUSE, JOYSTICK };
// 
// 		InputType	type;
// 		int			joystickID;
// 		float		directionCutTreshold;
// 		InputData	inputData;
// 
// 		std::vector< glm::vec2 > directionBuffer;
// 		btVector3	direction;
	};
}