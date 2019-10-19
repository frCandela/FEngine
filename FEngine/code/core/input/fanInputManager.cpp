#include "fanGlobalIncludes.h"
#include "core/input/fanInputManager.h"

#include "core/input/fanKeyboard.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Signal<>* InputManager::CreateKeyboardEvent( const std::string& _name, const Keyboard::Key _key, const Keyboard::Key _mod0, const Keyboard::Key _mod1, const  Keyboard::Key _mod2 ) {
		assert( FindEvent( _name ) == nullptr );



		KeyboardEvent keyEvent;
		keyEvent.key = _key;
		keyEvent.mod0 = _mod0;
		keyEvent.mod1 = _mod1;
		keyEvent.mod2 = _mod2;
		m_keyboardEvents[_name] = keyEvent;

		return & m_keyboardEvents[_name].onEvent;
	}

	//================================================================================================================================
	//================================================================================================================================
	void InputManager::CreateAxis( const std::string& _name, const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative ) {
		assert ( m_axis.find( _name ) == m_axis.end() );

		Axis axis;
		axis.keyNegative = _keyNegative;
		axis.keyPositive = _keyPositive;
		m_axis[_name] = axis;
	}

	//================================================================================================================================
	//================================================================================================================================
	float InputManager::GetAxis( const std::string& _name ) {
		assert ( m_axis.find( _name ) != m_axis.end() );
		Axis& axis = m_axis[_name];
		return  ( Keyboard::IsKeyDown( axis.keyPositive ) ? 1.f : 0.f ) + ( Keyboard::IsKeyDown( axis.keyNegative ) ? -1.f : 0.f );
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
	bool InputManager::Load( Json & /*_json*/ ) {

// 		for (int eventIndex = 0; eventIndex < _json.size(); eventIndex++) {
// 			Json& jEvent = _json[eventIndex];
// 
// 			std::string name;
// 			std::vector<Keyboard::Key> modifiers;
// 			Keyboard::Key key;
// 
// 			LoadString(jEvent, "name", name );
// 			LoadInt( jEvent, "key", key );
// 
// 			Json& jModifiers = _json["modifiers"];
// 			for (int keyIndex = 0; keyIndex < jModifiers.size(); keyIndex++) {
// 				Keyboard::Key modifierKey = jModifiers[keyIndex];
// 				modifiers.push_back( modifierKey );
// 			}
// 			//CreateKeyboardEvent( name, modifiers, key );
// 		}

		return true;
	}
		
	//================================================================================================================================
	//================================================================================================================================
	bool InputManager::Save( Json & /*_json*/ ) const {
// 		size_t index = 0;
// 		for ( auto keyEvent : m_keyboardEvents) {
// 			Json& jEvent = _json[index];
// 
// 			SaveString( jEvent, "name", keyEvent.first );
// 			SaveUInt(	jEvent, "key", keyEvent.second.key );
// 
// 			Json& jModifiers = _json["modifiers"];
// 			for ( int keyIndex = 0; keyIndex < keyEvent.second.modifiers.size(); keyIndex++ ) {
// 				SaveUInt( jModifiers[keyIndex], "key", keyEvent.second.modifiers[keyIndex] );
// 			}
// 
// 			++ index;
// 		}
		return true;
	}
}