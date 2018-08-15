#pragma once

#include <iostream>
#include <functional>
#include <vector>


/*!
\brief Signal is a communication system between widgets

A signal must be created in the emitter widget. A slot function to handle the signal must be created in the receiver.

The widgets are connected together using Signal< T >::connect

The signal is emitted using Signal< T >::emmit.
\param T arguments of the slot of the receiver
\warning Signals references and pointers MUST be initialized last.
\see Signal<void>
*/
template < typename... T > class Signal
{
public:
	//!< Emmit a signal = call the slots (functions) of the receiver.\param args arguments transmitted
	void emmit(T... args)			
	{
		for( auto slot : m_slots )
			slot(args...);
	}
	
	template <typename ClassA, typename ClassB>
	//!< Connect 2 widgets with a signal. \param slot adress of the slot fonction in the receiver. \param dst adress of the receiver.
	void connect(void(ClassA::* slot)(T...), ClassB* dst) 
	{
		auto s = std::function<void(T...)>(
			[=](T... args)
			{
				return (dst->*slot)(args...);
			}
		);
		m_slots.push_back(s);
	}

private:
	std::vector< std::function<void(T...)> > m_slots; //!< Vector of slots (functions) called when the signal is emmited
};

/*!
\brief No argument version of the class Signal
\see Signal
*/
template <> class Signal<void>
{
public:	
	//!< \brief Emmit a signal = call the slots (functions) of the receiver.
	void emmit()										
	{
		for ( auto slot : m_slots)
			slot();
	}

	
	template <typename ClassA, typename ClassB>	
	//!< Connect 2 widgets with a signal. \param slot adress of the slot (fonction) in the receiver. \param dst adress of the receiver.
	void connect(void(ClassA::* slot)(), ClassB* dst)
	{
		auto s =  std::function<void()>(
			[=]()
			{
					return (dst->*slot)();
			}
		);
		m_slots.push_back(s);
	}

private:
	std::vector< std::function<void()> > m_slots{}; //!< Vector of slots (functions) called when the signal is emmited
};