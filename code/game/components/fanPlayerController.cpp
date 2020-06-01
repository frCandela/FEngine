#include "game/components/fanPlayerController.hpp"

#include "core/fanSerializable.hpp"
#include "core/input/fanJoystick.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void PlayerController::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &PlayerController::OnGui;
		_info.load = &PlayerController::Load;
		_info.save = &PlayerController::Save;
		_info.editorPath = "game/player/";
		_info.name = "player_controller";
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerController::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& /*_component*/ )
	{
		//PlayerController& playerController = static_cast<PlayerController&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerController::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& /*_component*/ )
	{
//		PlayerController& playerController = static_cast<PlayerController&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
// 			// Input type
// 			int type = playerInput.type;
// 			if( ImGui::Combo( "input type", &type, "keyboard+mouse\0joystick\0" ) )
// 			{
// 				playerInput.type = InputType( type );
// 			}
// 
// 			ImGui::DragFloat( "fire", &playerInput.inputData.fire );
// 
// 			if( playerInput.type == JOYSTICK )
// 			{
// 				ImGui::SliderInt( "joystick ID", &playerInput.joystickID, 0, Joystick::NUM_JOYSTICK - 1 );
// 			}
// 
// 			// Direction buffer size
// 			int sizeBuffer = (int)playerInput.directionBuffer.size();
// 			if( ImGui::SliderInt( "direction buffer size", &sizeBuffer, 1, 64 ) )
// 			{
// 				playerInput.directionBuffer.resize( sizeBuffer, glm::vec2( 0 ) );
// 			}
// 
// 			// direction cut threshold
// 			ImGui::DragFloat( "direction cut threshold", &playerInput.directionCutTreshold, 0.01f, 0.f, 1.f );

		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerController::Load( EcsComponent& /*_component*/, const Json& /*_json*/ )
	{
//		PlayerController& playerController = static_cast<PlayerController&>( _component );
//
// 		int tmp;
// 		if( Serializable::LoadInt( _json, "direction_buffer_size", tmp ) )
// 		{
// 			playerInput.directionBuffer.resize( tmp, glm::vec3( 0.f ) );
// 		}
// 		Serializable::LoadFloat( _json, "direction_cut_treshold", playerInput.directionCutTreshold );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PlayerController::Save( const EcsComponent& /*_component*/, Json& /*_json*/ )
	{
//		const PlayerController& playerController = static_cast<const PlayerController&>( _component );

// 		Serializable::SaveInt( _json, "direction_buffer_size", (int)playerInput.directionBuffer.size() );
// 		Serializable::SaveFloat( _json, "direction_cut_treshold", playerInput.directionCutTreshold );
	}
}