#pragma once

#include "core/fanCorePrecompiled.hpp"
#include <stdlib.h>

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	template<typename T>
	class AlignedMemory
	{
	public:
		AlignedMemory() :
			m_data( nullptr )
			, m_size( 0 )
			, m_alignment( 0 )
		{}

		~AlignedMemory()
		{
			AlignedFree( m_data );
		}

		size_t	Alignment() const { return m_alignment; }
		void	SetAlignement( const size_t _alignment )
		{
			assert( std::_Is_pow_2( _alignment ) );
			assert( _alignment > sizeof( T ) && m_alignment >> 1 < sizeof( T ) );

			m_alignment = _alignment;
		}

		size_t	Size() const { return m_size; }
		void Resize( size_t _size )
		{
			assert( m_alignment > 0 );

			void* oldData = m_data;

			m_data = AlignedMalloc( _size * m_alignment, ( int ) m_alignment );

			if ( oldData != nullptr )
			{
				memcpy( m_data, oldData, m_size );
				AlignedFree( oldData );
				oldData = nullptr;
			}

			m_size = _size;
		}

		static constexpr size_t tmp = sizeof( void* );

		T& operator[]( const int& _pos )
		{
			assert( m_data != nullptr );
			void* adress = static_cast< char* >( m_data ) + m_alignment * _pos;
			return *static_cast< T* >( adress );
		}

	private:
		void* m_data;
		size_t m_size;
		size_t m_alignment;

		//================================================================================================================================
		//================================================================================================================================
		void* AlignedMalloc( size_t _size, size_t _alignment )
		{
			void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
			data = _aligned_malloc( _size, _alignment );
#else 
			int res = posix_memalign( &data, _alignment, _size );
			if( res != 0 )
				data = nullptr;
#endif
			return data;
		}

		//================================================================================================================================
		//================================================================================================================================
		void AlignedFree( void* _data )
		{
			if( _data != nullptr )
			{
#if	defined(_MSC_VER) || defined(__MINGW32__)
				_aligned_free( _data );
#else 
				free( _data );
#endif
			}
		}
	};
}
