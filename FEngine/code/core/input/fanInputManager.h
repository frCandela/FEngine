#pragma once

#include "core/fanSingleton.h"
#include "core/fanISerializable.h"
#include "core/input/fanKeyboard.h"


namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class InputManager : public ISerializable {
	public:
		//================================================================
		//================================================================
		struct KeyboardEvent {			
			Keyboard::Key key;
			Keyboard::Key mod0;
			Keyboard::Key mod1;
			Keyboard::Key mod2;
			Signal<> onEvent;
		};
		//================================================================
		//================================================================
		struct Axis {
			Keyboard::Key keyPositive;
			Keyboard::Key keyNegative;
		};

		//================================================================
		//================================================================
		void		CreateAxis( const std::string& _name,  const Keyboard::Key _keyPositive, const Keyboard::Key _keyNegative );
		Signal<>*	CreateKeyboardEvent( const std::string& _name, const Keyboard::Key _key, const Keyboard::Key _mod0 = Keyboard::NONE, const Keyboard::Key _mod1 = Keyboard::NONE, const  Keyboard::Key _mod2 = Keyboard::NONE );
		Signal<>*	FindEvent( const std::string& _name );
		float		GetAxis( const std::string& _name );

		std::map< std::string, Axis >&			GetListAxis()			{ return m_axis; }
		std::map< std::string, KeyboardEvent >&	GetListKeyboardEvents() { return m_keyboardEvents; }

		void		PullEvents();
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:

		std::map< std::string, Axis >			m_axis;
		std::map< std::string, KeyboardEvent >	m_keyboardEvents;
	};
}