#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// PlayerController can be the mouse/keyboard or a game pad
	// @wip gamepad is disabled for now
	//================================================================================================================================
	struct PlayerController : public EcsComponent
	{
		ECS_COMPONENT( PlayerController )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
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