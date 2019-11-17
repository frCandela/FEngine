#include "fanGlobalIncludes.h"
#include "core/input/fanInputManager.h"

#include "core/input/fanKeyboard.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Signal<>* InputManager::CreateKeyboardEvent( const std::string& _name, const Keyboard::Key _key, const Keyboard::Key _mod0, const Keyboard::Key _mod1, const  Keyboard::Key _mod2 ) {
		// Already exists
		Signal<>* keyEvent = FindEvent( _name );
		if ( keyEvent != nullptr ) {
			return &m_keyboardEvents[_name].onEvent;
		}

		// Creates new one
		KeyboardEvent newKeyEvent;
		newKeyEvent.key = _key;
		newKeyEvent.mod0 = _mod0;
		newKeyEvent.mod1 = _mod1;
		newKeyEvent.mod2 = _mod2;
		m_keyboardEvents[_name] = newKeyEvent;

		return & m_keyboardEvents[_name].onEvent;
	}

	//================================================================================================================================
	//================================================================================================================================
	void InputManager::CreateKeyboardAxis( const std::string& _name, const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative ) {
		// Already exists
		if( m_axis.find( _name ) != m_axis.end() ) {
			return;
		}

		// Creates new one
		Axis axis( _name, Axis::KEYBOARD );
		axis.SetKeyboardKeys(_keyPositive, _keyNegative );
		m_axis[_name] = axis;
	}

	//================================================================================================================================
	//================================================================================================================================
	float InputManager::GetAxis( const std::string& _name ) {
		assert ( m_axis.find( _name ) != m_axis.end() );
		Axis& axis = m_axis[_name];
		return  axis.GetValue();
	}

	//================================================================================================================================
	//================================================================================================================================
	Signal<>* InputManager::FindEvent( const std::string& _name ) {

		std::map< std::string, KeyboardEvent >::iterator it = m_keyboardEvents.find(_name);
		if ( it != m_keyboardEvents.end() ) {
			return & it->second.onEvent;
		}
		return nullptr;
	}	

	//================================================================================================================================
	// Iterates over all events and calls them if the necessary keys are pressed
	//================================================================================================================================
	void InputManager::PullEvents() {
		for ( auto& pair : m_keyboardEvents ) {
			KeyboardEvent& keyEvent = pair.second;

			bool modifiersPressed = ( keyEvent.mod0 == Keyboard::NONE ? true : Keyboard::IsKeyDown( keyEvent.mod0 ) ) &&		
									( keyEvent.mod1 == Keyboard::NONE ? true : Keyboard::IsKeyDown( keyEvent.mod1 ) ) &&
									( keyEvent.mod2 == Keyboard::NONE ? true : Keyboard::IsKeyDown( keyEvent.mod2 ) );

			if ( modifiersPressed && Keyboard::IsKeyPressed( keyEvent.key ) ) {
				keyEvent.onEvent.Emmit();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool InputManager::Load( Json & _json ) {
		{// Events
			size_t index = 0;
			Json& jEvents = _json["events"];
			for ( size_t eventIndex = 0; eventIndex < jEvents.size(); ++eventIndex ) {
				Json& jEvent_i = jEvents[index];

				std::string name;
				KeyboardEvent keyEvent;

				LoadString( jEvent_i, "name", name );
				LoadInt( jEvent_i, "key", keyEvent.key );
				LoadInt( jEvent_i, "mod0", keyEvent.mod0 );
				LoadInt( jEvent_i, "mod1", keyEvent.mod1 );
				LoadInt( jEvent_i, "mod2", keyEvent.mod2 );

				m_keyboardEvents[name] = keyEvent;

				++index;
			}
		}

		{ // Axis
			size_t index = 0;
			Json& jAxis = _json["axis"];
			for ( size_t axisIndex = 0; axisIndex < jAxis.size(); ++axisIndex ) {
				Json& jAxis_i = jAxis[index];
				
				Axis axis;
				axis.Load( jAxis_i );
				m_axis[axis.GetName()] = axis;

				++index;
			}
		}

		return true;
	}
		
	//================================================================================================================================
	//================================================================================================================================
	bool InputManager::Save( Json & _json ) const {
		{// Events
			size_t index = 0;
			Json& jEvents = _json["events"];
			for ( auto keyEvent : m_keyboardEvents ) {
				Json& jEvent_i = jEvents[index];
				SaveString( jEvent_i, "name", keyEvent.first );
				SaveInt( jEvent_i, "key", keyEvent.second.key );
				SaveInt( jEvent_i, "mod0", keyEvent.second.mod0 );
				SaveInt( jEvent_i, "mod1", keyEvent.second.mod1 );
				SaveInt( jEvent_i, "mod2", keyEvent.second.mod2 );
				++index;
			}
		}

		{ // Axis
			size_t index = 0;
			Json& jAxis = _json["axis"];
			for ( auto axisPair : m_axis ) {
				Json& jEvent_i = jAxis[index];
				axisPair.second.Save(jEvent_i);
				++index;
			}
		}
		return true;
	}
}