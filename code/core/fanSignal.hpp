#pragma once

#include <vector>

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	template < typename... T > class Signal
	{
	private:
		//================================================================
		struct SlotBase
		{
			virtual void Call( T... _args ) = 0;
		};

		//================================================================
		template <typename ClassType>
		class Slot : public SlotBase
		{
		public:
			Slot( void( ClassType::* _method )( T... ), ClassType* _object ) :
				m_object( _object ),
				m_method( _method )
			{}

			void Call( T... _args ) override { ( ( *m_object ).*( m_method ) )( _args... ); }

			bool Equals( void( ClassType::* const _method )( T... ), const ClassType* _object )
			{
				return m_object == _object && m_method == _method;
			}
		private:
			ClassType* m_object;
			void ( ClassType::* m_method )( T... );
		};

	public:
		~Signal()
		{
			for ( size_t slotIndex = 0; slotIndex < m_slots.size(); slotIndex++ )
			{
				delete( m_slots[ slotIndex ] );
			}
			m_slots.clear();
		}

		// Connects the signal to the member function _method of ClassType _object
		template <typename ClassType>
		void Connect( void( ClassType::* _method )( T... ), ClassType* _object )
		{
			m_slots.push_back( new Slot<ClassType>( _method, _object ) );
		}

		// Disconnect the signal from the the member function _method of ClassType _object
		template <typename ClassType>
		void Disconnect( void( ClassType::* _method )( T... ), ClassType* _object )
		{
			for ( size_t slotIndex = 0; slotIndex < m_slots.size(); slotIndex++ )
			{
				SlotBase* slotbase = m_slots[ slotIndex ];
				Slot<ClassType>* slot = static_cast< Slot<ClassType>* >( slotbase );

				if ( slot->Equals( _method, _object ) )
				{
					delete( slot );
					m_slots.erase( m_slots.begin() + slotIndex );
					return;
				}
			}
		}

		// Calls all slots connected to the signal
		void Emmit( T... _args )
		{
			for ( size_t slotIndex = 0; slotIndex < m_slots.size(); slotIndex++ )
			{
				SlotBase* slot = m_slots[ slotIndex ];
				slot->Call( _args... );
			}
		}

	private:
		std::vector<SlotBase*> m_slots;
	};
}