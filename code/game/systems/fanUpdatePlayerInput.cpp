#include "game/systems/fanUpdatePlayerInput.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanPlayerController.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "core/input/fanMouse.hpp"
#include "core/math/shapes/fanRay.hpp"

#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanJoystick.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_RefreshPlayerInput::GetSignature( const EcsWorld& _world )
	{
		return	
			_world.GetSignature<PlayerInput>() | 
			_world.GetSignature<Transform>() | 
			_world.GetSignature<PlayerController>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_RefreshPlayerInput::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const Scene& scene = _world.GetSingleton<Scene>();
		const EcsEntity cameraID = _world.GetEntity( scene.mainCameraSceneNode );
		const Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
		const Camera& camera = _world.GetComponent<Camera>( cameraID );

		auto transformIt = _view.begin<Transform>();
		auto inputIt = _view.begin<PlayerInput>();
		auto controllerIt = _view.begin<PlayerController>();
		for( ; transformIt != _view.end<Transform>(); ++transformIt, ++inputIt, ++controllerIt )
		{
			const Transform& transform = *transformIt;
			PlayerInput& input = *inputIt;
			const PlayerController& controller = *controllerIt;

			input.left =	Input::Get().Manager().GetAxis( "game_left" );
			input.forward = Input::Get().Manager().GetAxis( "game_forward" );
			input.boost =	Input::Get().Manager().GetAxis( "game_boost" );
			input.fire =	Input::Get().Manager().GetAxis( "game_fire" );

			btVector3 mouseWorldPos = camera.ScreenPosToRay( cameraTransform, Mouse::Get().GetScreenSpacePosition() ).origin; // Get mouse world pos
			mouseWorldPos.setY( 0 );			
			btVector3 mouseDir = mouseWorldPos - transform.GetPosition();// Get mouse direction
			mouseDir.normalize();
			input.orientation = mouseDir;
		}
	}
//================================================================================================================================
// 	 Old joystick code below
//================================================================================================================================
//
//
// 	if( input.type == fan::PlayerInput::JOYSTICK )
// 	{
// 		glm::vec2 average = GetDirectionAverage( input );
// 
// 		btVector3 dir = btVector3( average.x, 0.f, average.y );
// 
// 		if( dir.length() > input.directionCutTreshold ) { input.direction = dir; }
// 
// 		inputData.direction = input.direction;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	float S_RefreshPlayerInput::GetInputLeft( const PlayerInput& _input )
// 	{
// 		switch( _input.type )
// 		{
// 		case fan::PlayerInput::KEYBOARD_MOUSE:
// 			return Input::Get().Manager().GetAxis( "game_left", _input.joystickID );
// 		case fan::PlayerInput::JOYSTICK:
// 		{
// 			float rawInput = Input::Get().Manager().GetAxis( "gamejs_axis_left", _input.joystickID );
// 			return std::abs( rawInput ) > 0.15f ? rawInput : 0.f;
// 		}
// 		default:
// 			return 0.f;
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	float S_RefreshPlayerInput::GetInputForward( const PlayerInput& _input )
// 	{
// 		switch( _input.type )
// 		{
// 		case fan::PlayerInput::KEYBOARD_MOUSE:
// 			return Input::Get().Manager().GetAxis( "game_forward" );
// 		case fan::PlayerInput::JOYSTICK:
// 		{
// 			const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction" );
// 			const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", _input.joystickID );
// 			float forward = btVector3( x, 0.f, y ).length();
// 			return forward > 0.2f ? forward : 0.f;
// 		} default:
// 			return 0.f;
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	float S_RefreshPlayerInput::GetInputBoost( const PlayerInput& _input )
// 	{
// 		switch( _input.type )
// 		{
// 		case fan::PlayerInput::KEYBOARD_MOUSE:
// 			return Input::Get().Manager().GetAxis( "game_boost" );
// 		case fan::PlayerInput::JOYSTICK:
// 			return Input::Get().Manager().GetAxis( "gamejs_axis_boost", _input.joystickID );
// 		default:
// 			return 0.f;
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	float S_RefreshPlayerInput::GetInputFire( const PlayerInput& _input )
// 	{
// 		switch( _input.type )
// 		{
// 		case fan::PlayerInput::KEYBOARD_MOUSE:
// 			return Input::Get().Manager().GetAxis( "game_fire" );
// 		case fan::PlayerInput::JOYSTICK:
// 			return Input::Get().Manager().GetAxis( "gamejs_axis_fire", _input.joystickID );
// 		default:
// 			return 0.f;
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	glm::vec2 S_RefreshPlayerInput::GetDirectionAverage( PlayerInput& _input )
// 	{
// 		const size_t index = Input::Get().FrameCount() % _input.directionBuffer.size();
// 		const float x = Input::Get().Manager().GetAxis( "gamejs_x_axis_direction", _input.joystickID );
// 		const float y = Input::Get().Manager().GetAxis( "gamejs_y_axis_direction", _input.joystickID );
// 		_input.directionBuffer[index] = glm::vec2( x, y );
// 
// 		glm::vec2 average( 0.f, 0.f );
// 		for( int dirIndex = 0; dirIndex < _input.directionBuffer.size(); dirIndex++ )
// 		{
// 			average += _input.directionBuffer[dirIndex];
// 		}
// 		average /= float( _input.directionBuffer.size() );
// 
// 		return average;
// 	}
}