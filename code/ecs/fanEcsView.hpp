#pragma once

#include "ecs/fanEcsArchetype.hpp"

namespace fan
{
	//================================
	//================================
	struct EcsView
	{
		EcsView( const std::unordered_map<uint32_t, int >& _typesToIndex, const EcsSignature _signature ) :
			m_typesToIndex( _typesToIndex )
			, m_signature( _signature )
		{
		}

		const std::unordered_map<uint32_t, int >& m_typesToIndex;
		const EcsSignature m_signature;
		std::vector<EcsArchetype*> m_archetypes;


		//================================
		//================================
		template < typename _ComponentType >
		struct iterator
		{
			iterator( const EcsView& _view, const int _componentIndex )
			{
				if( _componentIndex == -1 )
				{
					m_archetypeIndex = uint16_t( _view.m_archetypes.size() ); // end() iterator
				}
				else
				{
					assert( _view.m_signature[_componentIndex] );
					m_archetypes = _view.m_archetypes;
					m_componentIndex = _componentIndex;
					m_chunkIndex = 0;
					m_elementIndex = 0;
					m_archetypeIndex = 0;
					if( !m_archetypes.empty() )
					{
						m_currentArchetype = m_archetypes[m_archetypeIndex];
						m_currentChunk = &m_currentArchetype->m_chunks[_componentIndex].GetChunk( m_chunkIndex );
					}
				}
			}

			inline bool operator!=( const iterator& _other ) const
			{
				return m_archetypeIndex != _other.m_archetypeIndex;
			}

			void operator++() // prefix ++
			{
				++m_elementIndex;
				if( m_elementIndex >= m_currentChunk->Size() )
				{
					m_elementIndex = 0;
					++m_chunkIndex;

					if( m_chunkIndex < m_currentArchetype->m_chunks[m_componentIndex].NumChunk() )
					{
						m_currentChunk = &m_currentArchetype->m_chunks[m_componentIndex].GetChunk( m_chunkIndex );
					}
					else
					{
						m_chunkIndex = 0;
						++m_archetypeIndex;
						if( m_archetypeIndex < m_archetypes.size() )
						{
							m_currentArchetype = m_archetypes[m_archetypeIndex];
							m_currentChunk = &m_currentArchetype->m_chunks[m_componentIndex].GetChunk( m_chunkIndex );
						}
					}
				}
			}

			inline _ComponentType& operator*()
			{
				return *static_cast<_ComponentType*>( m_currentChunk->At( m_elementIndex ) );
			}

			EcsEntity Entity() const
			{
				const uint32_t index = m_chunkIndex * m_currentChunk->Capacity() + m_elementIndex;
				return { m_currentArchetype, index };
			}
		private:
			std::vector< EcsArchetype* >	m_archetypes;
			int								m_componentIndex;
			uint16_t						m_archetypeIndex;
			uint16_t						m_chunkIndex;
			uint16_t						m_elementIndex;
			EcsArchetype* m_currentArchetype;
			EcsChunk* m_currentChunk;
		};

		template < typename _ComponentType >
		iterator<_ComponentType> begin() const
		{
			const int index = m_typesToIndex.at( _ComponentType::Info::s_type );
			return iterator<_ComponentType>( *this, index );
		}

		template < typename _ComponentType >
		inline iterator<_ComponentType> end() const
		{
			return iterator<_ComponentType>( *this, -1 );
		}

		int Size() const
		{
			int size = 0;
			for( int i = 0; i < m_archetypes.size(); i++ )
			{
				size += m_archetypes[i]->Size();
			}
			return size;
		}

		bool Empty() const
		{
			return Size() == 0;
		}
	};
}