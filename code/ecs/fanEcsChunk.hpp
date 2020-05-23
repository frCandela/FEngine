#pragma once

#include <memory>
#include <cassert>

namespace fan
{
	//================================
	// Small aligned memory buffer
	//================================
	class EcsChunk
	{
	public:
		static constexpr size_t ecsChunkMaxSize = 65536;

		void Create( void* ( *_cpyFunction )( void*, const void*, size_t ),  const int _componentSize, const int _alignment )
		{
			m_cpyFunction = _cpyFunction;
			m_componentSize = _componentSize;
			m_capacity = ecsChunkMaxSize / _componentSize;
			m_size = 0;

			int chunckSize = m_capacity * _componentSize;
			m_buffer = new uint8_t[chunckSize + _alignment];

			size_t space = chunckSize + _alignment;
			m_alignedBuffer = m_buffer;
			std::align( _alignment, _componentSize, m_alignedBuffer, space );
		}

		void Destroy()
		{
			delete m_buffer;
			m_buffer = nullptr;
			m_alignedBuffer = nullptr;
		}

		bool	Empty() const { return m_size == 0; }
		bool	Full() const { return m_size == m_capacity; }
		int		Size() const { return m_size; }
		int		Capacity() const { return m_capacity; }

		void* At( const int _index )
		{
			assert( _index < m_size );
			uint8_t* buffer = static_cast<uint8_t*>( m_alignedBuffer );
			return &buffer[_index * m_componentSize];
		}

		void Set( const int _index, void* _data )
		{
			assert( _index < m_size );
			m_cpyFunction( At( _index ), _data, m_componentSize );
		}

		void Remove( const int _index )
		{
			assert( _index < m_size );
			// back swap
			if( m_size != 1 && _index != m_size - 1 )
			{
				Set( _index, At( m_size - 1 ) );
			}
			m_size--;
		}

		void PushBack( void* _data )
		{
			assert( m_size < m_capacity );
			const int index = m_size;
			m_size++;
			Set( index, _data );
		}

		void PopBack()
		{
			assert( m_size > 0 );
			m_size--;
		}

		void EmplaceBack()
		{
			assert( m_size < m_capacity );
			m_size++;
		}

		void Clear()
		{
			m_size = 0;
		}

	private:
		int m_capacity = 0;
		int m_size;
		int m_componentSize;
		void* m_buffer;
		void* m_alignedBuffer;
		void* ( *m_cpyFunction )( void*, const void*, size_t ) = nullptr;
	};
}