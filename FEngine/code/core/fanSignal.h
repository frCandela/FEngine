#pragma once

#include <vector>
#include <functional>

namespace util {

	template < typename... T > class Signal
	{
	public:
		void Emmit(T... args)
		{
			for (auto slot : m_slots)
				slot(args...);
		}

		template <typename ClassA, typename ClassB>
		//!< Connect the Signal to the method dst of  class
		void Connect(void(ClassA::* slot)(T...), ClassB* dst)
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


	// No argument version of the class Signal
	template <> class Signal<void>
	{
	public:
		//!< \brief Emmit a signal = call the slots (functions) of the receiver.
		void Emmit()
		{
			for (auto slot : m_slots)
				slot();
		}


		template <typename ClassA, typename ClassB>
		//!< Connect 2 widgets with a signal. \param slot adress of the slot (fonction) in the receiver. \param dst adress of the receiver.
		void Connect(void(ClassA::* slot)(), ClassB* dst)
		{
			auto s = std::function<void()>(
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
}