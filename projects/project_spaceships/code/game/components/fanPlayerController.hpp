#pragma once

#include "core/ecs/fanEcsComponent.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	// PlayerController can be the mouse/keyboard or a game pad
	// It is automatically placed on the spaceship in client mode to update the player input
	// @wip gamepad is disabled for now
	//========================================================================================================
	struct PlayerController : public EcsComponent
	{
		ECS_COMPONENT( PlayerController )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

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