#pragma once

#include <memory>
#include <cassert>

#include "fanEcsChunkAllocator.hpp"

namespace fan
{
	//================================
	// Small aligned memory buffer
	//================================
	class EcsChunk
	{
	public:
		static EcsChunkAllocator s_allocator;

		void	Create( void* ( *_cpyFunction )( void*, const void*, size_t ), const int _componentSize, const int _alignment );
		void	Destroy();
		bool	Empty() const;
		bool	Full() const;
		int		Size() const;
		int		Capacity() const;
		void*	At( const int _index );
		void	Set( const int _index, void* _data );
		void	Remove( const int _index );
		void	PushBack( void* _data );
		void	PopBack();
		void	EmplaceBack();
		void	Clear();
	private:

		int		m_capacity = 0;
		int		m_size = 0;
		int		m_componentSize = 0;
		void*	m_buffer = nullptr;
		void*	m_alignedBuffer = nullptr;
		void* ( *m_cpyFunction )( void*, const void*, size_t ) = nullptr;
	};
}