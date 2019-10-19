#pragma once

#include "core/fanSingleton.h"
#include "core/fanISerializable.h"
#include "core/input/fanKeyboard.h"


namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class InputManager : public ISerializable {
	public:
		void		CreateAxis( const std::string& _name,  const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative );
		Signal<>*	CreateKeyboardEvent( const std::string& _name, const std::vector<Keyboard::Key> _modifiers, const Keyboard::Key _key );


		Signal<>*	FindEvent( const std::string& _name );
		float		GetAxis( const std::string& _name );

		void		PullEvents();
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		struct KeyboardEvent {
			std::vector<Keyboard::Key> modifiers;
			Keyboard::Key key;
			Signal<> onEvent;
		};
		struct Axis{
			Keyboard::Key keyPositive;
			Keyboard::Key keyNegative;
		};

		std::map< std::string, Axis > m_axis;
		std::map< std::string, KeyboardEvent > m_keyboardEvents;
	};
}