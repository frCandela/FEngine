#include "fanGlobalIncludes.h"
#include "core/input/fanEventManager.h"

#include "core/input/fanKeyboard.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Signal<>* EventManager::CreateKeyboardEvent( const std::string& _name, const std::vector<Keyboard::Key> _modifiers, const Keyboard::Key _key ) {
		assert( FindEvent( _name ) == nullptr );

		KeyboardEvent keyEvent;
		keyEvent.modifiers = _modifiers;
		keyEvent.key = _key;
		m_keyboardEvents[_name] = keyEvent;

		return & m_keyboardEvents[_name].onEvent;
	}
	//================================================================================================================================
	//================================================================================================================================
	Signal<>* EventManager::FindEvent( const std::string& _name ) {

		std::map< std::string, KeyboardEvent >::iterator it = m_keyboardEvents.find(_name);
		if ( it != m_keyboardEvents.end() ) {
			return & it->second.onEvent;
		}
		return nullptr;
	}	

	//================================================================================================================================
	// Iterates over all events and calls them if the necessary keys are pressed
	//================================================================================================================================
	void EventManager::PullEvents() {
		for ( auto& pair : m_keyboardEvents ) {
			KeyboardEvent& keyEvent = pair.second;

			bool modifiersPressed = true;
			for (int modifierIndex = 0; modifierIndex < keyEvent.modifiers.size(); modifierIndex++)	{
				if ( ! Keyboard::IsKeyDown( keyEvent.modifiers[modifierIndex] ) ) {
					modifiersPressed = false;
					continue;
				}
			}
			if ( modifiersPressed && Keyboard::IsKeyPressed( keyEvent.key ) ) {
				keyEvent.onEvent.Emmit();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool EventManager::Load( Json & _json ) {

		for (int eventIndex = 0; eventIndex < _json.size(); eventIndex++) {
			Json& jEvent = _json[eventIndex];

			std::string name;
			std::vector<Keyboard::Key> modifiers;
			Keyboard::Key key;

			LoadString(jEvent, "name", name );
			LoadInt( jEvent, "key", key );

			Json& jModifiers = _json["modifiers"];
			for (int keyIndex = 0; keyIndex < jModifiers.size(); keyIndex++) {
				Keyboard::Key modifierKey = jModifiers[keyIndex];
				modifiers.push_back( modifierKey );
			}
			CreateKeyboardEvent( name, modifiers, key );
		}

		return true;
	}
		
	//================================================================================================================================
	//================================================================================================================================
	bool EventManager::Save( Json & _json ) const {
		size_t index = 0;
		for ( auto keyEvent : m_keyboardEvents) {
			Json& jEvent = _json[index];

			SaveString( jEvent, "name", keyEvent.first );
			SaveUInt(	jEvent, "key", keyEvent.second.key );

			Json& jModifiers = _json["modifiers"];
			for ( int keyIndex = 0; keyIndex < keyEvent.second.modifiers.size(); keyIndex++ ) {
				SaveUInt( jModifiers[keyIndex], "key", keyEvent.second.modifiers[keyIndex] );
			}

			++ index;
		}
		return true;
	}
}