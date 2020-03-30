#pragma once

#include <vector>

namespace fan
{
	namespace impl
	{
		//================================================================================================================================
		//================================================================================================================================
		template < typename... _Args >
		struct SlotBase
		{
			virtual void Call( _Args... _args ) = 0;
		};

		//================================================================================================================================
		//================================================================================================================================
		template <typename _Target, typename... _Args >
		class Slot : public SlotBase<_Args...>
		{
		public:
			Slot( void( _Target::* _method )( _Args... ), _Target* _object ) :
				m_object( _object ),
				m_method( _method )
			{}

			void Call( _Args... _args ) override { ( ( *m_object ).*( m_method ) )( _args... ); }

			bool Equals( void( _Target::* const _method )( _Args... ), const _Target* _object )
			{
				return m_object == _object && m_method == _method;
			}
		private:
			_Target* m_object;
			void ( _Target::* m_method )( _Args... );
		};
	}

	//================================================================================================================================
	// Signal
	//================================================================================================================================
	template < typename... _Args > class Signal
	{
	public:
		~Signal();
		
		template <typename _Target>
		void Connect( void( _Target::* _method )( _Args... ), _Target* _object );
		template <typename _Target>
		void Disconnect( void( _Target::* _method )( _Args... ), _Target* _object );
		void Emmit( _Args... _args );
		void Clear(){ m_slots.clear(); }

	private:
		std::vector<impl::SlotBase< _Args...>*> m_slots;
	};

	//================================================================================================================================
	// destructor
	//================================================================================================================================
	template < typename...  _Args >
	Signal< _Args...>::~Signal()
	{
		for( size_t slotIndex = 0; slotIndex < m_slots.size(); slotIndex++ )
		{
			delete( m_slots[slotIndex] );
		}
		m_slots.clear();
	}

	//================================================================================================================================
	// calls all slots connected to the signal
	//================================================================================================================================
	template < typename...  _Args >
	void Signal< _Args...>::Emmit( _Args... _args )
	{
		for( size_t slotIndex = 0; slotIndex < m_slots.size(); slotIndex++ )
		{
			impl::SlotBase< _Args...>* slot = m_slots[slotIndex];
			slot->Call( _args... );
		}
	}

	//================================================================================================================================
	// disconnect the signal from the the member function _method of _Target object
	//================================================================================================================================
	template < typename...  _Args >
	template <typename _Target>
	void Signal< _Args...>::Disconnect( void( _Target::* _method )( _Args... ), _Target* _object )
	{
		for( size_t slotIndex = 0; slotIndex < m_slots.size(); slotIndex++ )
		{
			impl::SlotBase< _Args...>* slotbase = m_slots[slotIndex];
			impl::Slot<_Target, _Args...>* slot = static_cast<impl::Slot<_Target, _Args...>*>( slotbase );

			if( slot->Equals( _method, _object ) )
			{
				delete( slot );
				m_slots.erase( m_slots.begin() + slotIndex );
				return;
			}
		}
	}

	//================================================================================================================================
	// connects the signal to the member function _method of _Target _object
	//================================================================================================================================
	template < typename...  _Args >
	template <typename _Target>
	void Signal< _Args...>::Connect( void( _Target::* _method )( _Args... ), _Target* _object )
	{
		m_slots.push_back( new impl::Slot<_Target, _Args...>( _method, _object ) );
	}
}