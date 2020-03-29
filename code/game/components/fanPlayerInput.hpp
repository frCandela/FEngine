#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "network/packets/fanPacketPlayerInput.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// manages the input of the player
	// this input can be direct in case of a local player or replicated of other players
	//================================================================================================================================
	struct PlayerInput : public Component
	{
		DECLARE_COMPONENT( PlayerInput )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		enum InputType { KEYBOARD_MOUSE, JOYSTICK };

		InputType	type;
		int			joystickID;
		float		directionCutTreshold;
		bool		isReplicated;
		InputData	inputData;

		std::vector< glm::vec2 > directionBuffer;
		btVector3	direction;
	};
}