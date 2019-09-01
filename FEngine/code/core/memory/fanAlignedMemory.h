#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	template<typename T>
	class AlignedMemory
	{
	public:
		AlignedMemory() : 
			m_data( nullptr )
			,m_size(0)
			,m_alignment(0)
			,m_ratio(0){
		}

		~AlignedMemory() {
			AlignedFree(m_data);
			m_data = nullptr;
		}

		void Resize(size_t _size, size_t _alignment) {
			assert(_alignment >= sizeof(T));

			AlignedFree(m_data);
			m_data = nullptr;
			m_size = _size;
			m_alignment = _alignment;

			m_ratio = m_alignment / sizeof(uint32_t);
			m_data = AlignedAlloc(_size, _alignment);
		}
		size_t GetSize() const		{ return m_size; }
		size_t GetAlignment() const { return m_alignment; }

		T& operator[](const int& _pos) {
			assert(m_data != nullptr);
			void * uint32Adress = static_cast<uint32_t*>(m_data) + _pos * m_ratio;
			return *static_cast<T*>(uint32Adress);
		}

	private:
		void * m_data;
		size_t m_size; 
		size_t m_alignment;
		size_t m_ratio;

		// Wrapper functions for aligned memory allocation
		// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
		void* AlignedAlloc(size_t _size, size_t _alignment);
		void AlignedFree(void* _data);
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename T>
	void* AlignedMemory<T>::AlignedAlloc(size_t _size, size_t _alignment)
	{
		void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
		data = _aligned_malloc(_size, _alignment);
#else 
		int res = posix_memalign(&data, _alignment, _size);
		if (res != 0)
			data = nullptr;
#endif
		return data;
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename T>
	void AlignedMemory<T>::AlignedFree(void* _data)
	{
		if (_data != nullptr) {
#if	defined(_MSC_VER) || defined(__MINGW32__)
			_aligned_free(_data);
#else 
			free(_data);
#endif
		}
	}
}
