#include "project_spaceships/components/fanPlayerController.hpp"

#include "core/input/fanJoystick.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PlayerController::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &PlayerController::Load;
		_info.save        = &PlayerController::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void PlayerController::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& /*_component*/ )
	{
		//PlayerController& playerController = static_cast<PlayerController&>( _component );

	}

	//========================================================================================================
	//========================================================================================================
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

	//========================================================================================================
	//========================================================================================================
	void PlayerController::Save( const EcsComponent& /*_component*/, Json& /*_json*/ )
	{
//		const PlayerController& playerController = static_cast<const PlayerController&>( _component );

// 		Serializable::SaveInt( _json, "direction_buffer_size", (int)playerInput.directionBuffer.size() );
// 		Serializable::SaveFloat( _json, "direction_cut_treshold", playerInput.directionCutTreshold );
	}
}