#include "core/input/fanInputManager.hpp"

#include "fanJson.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
    Signal<>* InputManager::CreateKeyboardEvent( const std::string& _name,
                                                 const Keyboard::Key _key,
                                                 const Keyboard::Key _mod0,
                                                 const Keyboard::Key _mod1,
                                                 const Keyboard::Key _mod2 )
    {
		// Already exists
		Signal<>* keyEvent = FindEvent( _name );
		if ( keyEvent != nullptr )
		{
			return &mKeyboardEvents[ _name ].onEvent;
		}

		// Creates new one
		KeyboardEvent newKeyEvent;
		newKeyEvent.key = _key;
		newKeyEvent.mod0 = _mod0;
		newKeyEvent.mod1 = _mod1;
		newKeyEvent.mod2 = _mod2;
		mKeyboardEvents[ _name ] = newKeyEvent;

		return &mKeyboardEvents[ _name ].onEvent;
	}

	//========================================================================================================
	//========================================================================================================
    void InputManager::CreateKeyboardAxis( const std::string& _name,
                                           const Keyboard::Key _keyPositive,
                                           const Keyboard::Key _keyNegative )
	{
		// Already exists
		if ( mAxis.find( _name ) != mAxis.end() )
		{
			return;
		}

		// Creates new one
		Axis axis( _name, Axis::Keyboard );
		axis.SetFromKeyboardKeys( _keyPositive, _keyNegative );
        mAxis[ _name ] = axis;
	}

	//========================================================================================================
	//========================================================================================================
    void InputManager::CreateJoystickAxis( const std::string& _name,
                                           const Joystick::JoystickID _GLFW_JOYSTICK,
                                           const Joystick::Axis _axis )
	{
		// Already exists
		if ( mAxis.find( _name ) != mAxis.end() )
		{
			Debug::Warning( "Axis already exists" );
			return;
		}

		// Creates new one
		Axis axis( _name, Axis::JoystickAxis );
		axis.SetFromJoystickAxis( _GLFW_JOYSTICK, _axis );
        mAxis[ _name ] = axis;
	}

	//========================================================================================================
    //========================================================================================================
    void InputManager::CreateJoystickButtons( const std::string& _name,
                                              const Joystick::JoystickID _GLFW_JOYSTICK,
                                              const Joystick::Button _positive,
                                              const Joystick::Button _negative )
    {
        // Already exists
		if ( mAxis.find( _name ) != mAxis.end() )
		{
			Debug::Warning( "Axis already exists" );
			return;
		}

		// Creates new one
		Axis axis( _name, Axis::JoystickButton );
		axis.SetFromJoystickButtons( _GLFW_JOYSTICK, _positive, _negative );
        mAxis[ _name ] = axis;
	}

	//========================================================================================================
	//========================================================================================================
	float InputManager::GetAxis( const std::string& _name, const int _joystickIDOverride )
	{
        fanAssert( mAxis.find( _name ) != mAxis.end() );
		Axis& axis = mAxis[ _name ];
		return  axis.GetValue( _joystickIDOverride );
	}

	//========================================================================================================
	//========================================================================================================
	Signal<>* InputManager::FindEvent( const std::string& _name )
	{

		std::map< std::string, KeyboardEvent >::iterator it = mKeyboardEvents.find( _name );
		if ( it != mKeyboardEvents.end() )
		{
			return &it->second.onEvent;
		}
		return nullptr;
	}

	//========================================================================================================
	// Iterates over all events and calls them if the necessary keys are pressed
	//========================================================================================================
	void InputManager::PullEvents()
	{
		for ( auto& pair : mKeyboardEvents )
		{
			KeyboardEvent& keyEvent = pair.second;

            bool modifiersPressed = ( keyEvent.mod0 == Keyboard::NONE
                    ? true
                    : Keyboard::IsKeyDown( keyEvent.mod0 ) ) &&
				( keyEvent.mod1 == Keyboard::NONE ? true : Keyboard::IsKeyDown( keyEvent.mod1 ) ) &&
				( keyEvent.mod2 == Keyboard::NONE ? true : Keyboard::IsKeyDown( keyEvent.mod2 ) );

			if ( modifiersPressed && Keyboard::IsKeyPressed( keyEvent.key ) )
			{
				keyEvent.onEvent.Emmit();
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	bool InputManager::Load( const Json& _json )
	{
		if( _json.is_null() )
		{
			return false;
		}

		{// Events
			size_t index = 0;
			const Json& jEvents = _json[ "events" ];
			for ( size_t eventIndex = 0; eventIndex < jEvents.size(); ++eventIndex )
			{
				const Json& jEvent_i = jEvents[ index ];

				std::string name;
				KeyboardEvent keyEvent;

				Serializable::LoadString( jEvent_i, "name", name );
				Serializable::LoadInt( jEvent_i, "key", keyEvent.key );
				Serializable::LoadInt( jEvent_i, "mod0", keyEvent.mod0 );
				Serializable::LoadInt( jEvent_i, "mod1", keyEvent.mod1 );
				Serializable::LoadInt( jEvent_i, "mod2", keyEvent.mod2 );

				mKeyboardEvents[ name ] = keyEvent;

				++index;
			}
		}

		{ // Axis
			size_t index = 0;
			const Json& jAxis = _json[ "axis" ];
			for ( size_t axisIndex = 0; axisIndex < jAxis.size(); ++axisIndex )
			{
				const Json& jAxis_i = jAxis[ index ];

				Axis axis;
				axis.Load( jAxis_i );
                mAxis[ axis.GetName() ] = axis;

				++index;
			}
		}

		return true;
	}

	//========================================================================================================
	//========================================================================================================
	bool InputManager::Save( Json& _json ) const
	{
		{// Events
			size_t index = 0;
			Json& jEvents = _json[ "events" ];
			for ( auto keyEvent : mKeyboardEvents )
			{
				Json& jEvent_i = jEvents[ index ];
				Serializable::SaveString( jEvent_i, "name", keyEvent.first );
				Serializable::SaveInt( jEvent_i, "key", keyEvent.second.key );
				Serializable::SaveInt( jEvent_i, "mod0", keyEvent.second.mod0 );
				Serializable::SaveInt( jEvent_i, "mod1", keyEvent.second.mod1 );
				Serializable::SaveInt( jEvent_i, "mod2", keyEvent.second.mod2 );
				++index;
			}
		}

		{ // Axis
			size_t index = 0;
			Json& jAxis = _json[ "axis" ];
			for ( auto axisPair : mAxis )
			{
				Json& jEvent_i = jAxis[ index ];
				axisPair.second.Save( jEvent_i );
				++index;
			}
		}
		return true;
	}
}