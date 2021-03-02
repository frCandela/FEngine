/// \file       OWLStaticArray.hpp
/// \date       08/05/2020
/// \project    OWL Engine
/// \package    Engine/Engine/Core
/// \author     Vincent STEHLY--CALISTO

#pragma once

// C++ feature
// C++ 11
#if __cplusplus >= 201103L
    #define _NOEXCEPT noexcept
    #define _NORETURN [[noreturn]

    // C++14
    #if __cplusplus >= 201403L
        #define _DEPRECATED(Message) [[deprecated(Message)]]
    #else
        #define _DEPRECATED(Message)
    #endif

    // C++17
    #if __cplusplus >= 201703L
        #define _CONSTEXPR17 constexpr
        #define _NODISCARD [[nodiscard]]
        #define _UNUSED [[maybe_unused]]
    #else
        #define _CONSTEXPR17
        #define _NODISCARD
        #define _UNUSED
    #endif

    // C++20 (not used yet) but there're interesting features
    #if __cplusplus > 201703L
    #endif
#else
    #define _NOEXCEPT
    #define _NORETURN
    #define _DEPRECATED(Message)
    #define _CONSTEXPR17
    #define _NODISCARD
    #define _UNUSED
#endif

#include <limits>
#include <memory>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace fan
{

/// \class   StaticArray
/// \brief   Fixed capacity array with aligned storage with a variable size [0, N]. This is very useful to optimize
///          memory allocations when we know approximately the size of containers (useful for stack allocators)
/// \tparam  T The underlying object type to store.
/// \tparam  N The number of objects to store.
/// \tparam  Alignment The underlying alignment of the buffer (optional, default is aligned on T)
/// \note    STL interface inspired from standard.
#if __cpp_lib_type_trait_variable_templates
    template <class T, uint64_t N, uint64_t Alignment = std::alignment_of_v<T>> class StaticArray
#else
    template <class T, uint64_t N, uint64_t Alignment = std::alignment_of<T>::value> class StaticArray
#endif
{
public:
     using value_type             = T;
     using size_type              = uint64_t;
     using storage_type           = uint8;
     using reference              = value_type&;
     using const_reference        = const value_type&;
     using pointer                = value_type*;
     using const_pointer          = const value_type*;
     using iterator               = value_type*;
     using const_iterator         = const value_type*;
     using reverse_iterator       = std::reverse_iterator<iterator>;
     using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    /// \brief  Default constructor
    StaticArray()
    : m_alignedStorage()
    , m_size          (0)
    {}

    /// \brief  Fill constructor. Fills the array with n default elements.
    /// \param  n The number of element to construct.
    explicit StaticArray(size_type n)
    : m_alignedStorage()
    , m_size          (0)
    {
        CapacityCheck(n);
        ConstructRangeValue(begin(), begin() + n);
        m_size = n;
    }

    /// \brief  Fill constructor. Fills the array with n elements. Each element is a copy of value.
    /// \param  n The number of element to construct.
    /// \param  value The value to initialize the elements from.
    StaticArray(size_type n, const_reference value)
    : m_alignedStorage()
    , m_size          (0)
    {
        CapacityCheck(n);
        ConstructRangeValue(begin(), begin() + n, value);
        m_size = n;
    }

    /// \brief  Range constructor. Constructs a container with as many elements as the range [first,last),
    ///         with each element constructed from its corresponding element in that range, in the same order.
    /// \param  first The start range iterator.
    /// \param  last The end of the range iterator.
    template <class InputIterator>
    StaticArray(InputIterator first, InputIterator last)
    : m_alignedStorage()
    , m_size          (0)
    {
        Assign(first, last);
    }

    /// \brief  Copy constructor. Constructs a container with a copy of each of the elements in x, in the same order.
    /// \param  other The other container to copy.
    StaticArray(const StaticArray& other) _NOEXCEPT
    : m_alignedStorage()
    , m_size          (0)
    {
        Assign(other.cbegin(), other.cend());
    }

    /// \brief  Initializer list constructor. Constructs a container with a copy of each
    ///         of the elements in il, in the same order.
    /// \param  il The initialize list.
    StaticArray (std::initializer_list<value_type> il)
    : m_alignedStorage()
    , m_size          (0)
    {
        Assign(il.begin(), il.end());
    }

    /// \brief  Destructor.
    ~StaticArray() _NOEXCEPT
    { Clear(); }

    /// \brief  Assignment operator. Copies the content of another static array into the current.
    /// \param  other The other static array to copy.
    /// \return A reference on this container.
    StaticArray& operator=(const StaticArray& other)
    {
        Assign(other.cbegin(), other.cend());
        return (*this);
    }

    /// \brief  Assignment operator. Copies the elements of il into the container.
    /// \param  The initializer list to copy.
    /// \return A reference on this container.
    StaticArray& operator=(std::initializer_list<value_type> il)
    {
        Assign(il.begin(), il.end());
        return (*this);
    }

    /// \brief  Assigns the content from a container to this one.
    /// \param  first The start of the range iterator.
    /// \parma  last The end of the range iterator.
    template<class InputIterator>
    void Assign(InputIterator first, InputIterator last)
    {
        const auto size = static_cast<size_type>(std::distance(first, last));

        CapacityCheck(size);
        if(m_size == size)
        {
            // Ideal case, same size
            std::copy(first, last, begin());
        }
        else if(m_size > size)
        {
            // Copy the existing range and then destroys the extra range
            std::copy(first, last, begin());
            DestroyRange(begin() + size, end());
        }
        else
        {
            // Constructs in place the missing range
            // Then copy the first part
            ConstructRangeIterator(first + m_size, last, begin() + m_size);
            std::copy(first, first + m_size, begin());
        }

        m_size = size;
    }

    /// \brief  Returns an iterator on the first element. If the container is empty, the iterator is invalid.
    /// \return An iterator on the first element.
    /// \note   Drops the Pascal case convention for iterator methods.
    _NODISCARD _CONSTEXPR17 iterator begin() _NOEXCEPT
    { return reinterpret_cast<iterator>(m_alignedStorage); }

    /// \brief  Returns a const iterator on the first element. If the container is empty, the iterator is invalid.
    /// \return A const iterator on the first element.
    _NODISCARD _CONSTEXPR17 const_iterator begin() const _NOEXCEPT
    { return reinterpret_cast<const_iterator>(m_alignedStorage); }

    /// \brief  Returns an iterator on the last element. If the container is empty, the iterator is invalid.
    /// \return An iterator on the last element.
    _NODISCARD iterator end() noexcept
    { return reinterpret_cast<iterator>(m_alignedStorage) + m_size; }

    /// \brief  Returns a const iterator on the last element. If the container is empty, the iterator is invalid.
    /// \return A const iterator on the last element.
    _NODISCARD const_iterator end() const noexcept
    { return reinterpret_cast<const_iterator>(m_alignedStorage) + m_size; }

    /// \brief  Returns a reverse iterator on the first element. If the container is empty, the iterator is invalid.
    /// \return A reverse iterator on the first element.
    _NODISCARD reverse_iterator rbegin() _NOEXCEPT
    { return reverse_iterator(end()); }

    /// \brief  Returns a const reverse iterator on the first element. If the container is empty, the iterator is invalid.
    /// \return A const reverse iterator on the first element.
    _NODISCARD const_reverse_iterator rbegin() const _NOEXCEPT
    { return const_reverse_iterator(cend()); }

    /// \brief  Returns a reverse iterator on the last element. If the container is empty, the iterator is invalid.
    /// \return A reverse iterator on the last element.
    _NODISCARD _CONSTEXPR17 reverse_iterator rend() _NOEXCEPT
    { return reverse_iterator(begin()); }

    /// \brief  Returns a const reverse iterator on the last element. If the container is empty, the iterator is invalid.
    /// \return A const reverse iterator on the last element.
    _NODISCARD _CONSTEXPR17 const_reverse_iterator rend() const _NOEXCEPT
    { return const_reverse_iterator(cbegin()); }

    /// \brief  Returns a const iterator on the first element. If the container is empty, the iterator is invalid.
    /// \return A const iterator on the first element.
    _NODISCARD _CONSTEXPR17 const_iterator cbegin() const _NOEXCEPT
    { return reinterpret_cast<const_iterator>(m_alignedStorage); }

    /// \brief  Returns a const iterator on the last element. If the container is empty, the iterator is invalid.
    /// \return A const iterator on the last element.
    _NODISCARD const_iterator cend() const _NOEXCEPT
    { return reinterpret_cast<const_iterator>(m_alignedStorage) + m_size; }

    /// \brief  Returns a const reverse iterator on the first element. If the container is empty, the iterator is invalid.
    /// \return A const reverse iterator on the first element
    _NODISCARD const_reverse_iterator crbegin() const _NOEXCEPT
    { return const_reverse_iterator(crend()); }

    /// \brief  Returns a const reverse iterator on the last element. If the container is empty, the iterator is invalid.
    /// \return A const reverse iterator on the last element
    _NODISCARD _CONSTEXPR17 const_reverse_iterator crend() const _NOEXCEPT
    { return const_reverse_iterator(crbegin()); }

    /// \brief  Returns the current number of elements in this container.
    /// \return The current number of elements in this container.
    _NODISCARD size_type Size() const _NOEXCEPT
    { return m_size; }

    /// \brief  Returns the max possible size of this container.
    /// \return The max possible size of this container..
    _NODISCARD _CONSTEXPR17 size_type MaxSize() const _NOEXCEPT
    { return std::numeric_limits<size_type>::max(); }

    /// \brief  Returns the max possible size of this container.
    /// \return The max possible size of this container.
    _NODISCARD _CONSTEXPR17 size_type Capacity() const _NOEXCEPT
    { return N; }

    /// \brief  Resizes the container so that it contains n elements.
    ///         If n is smaller than the current container size, the content is reduced to its first n elements,
    ///         removing those beyond (and destroying them). If n is greater than the current container size,
    ///         the content is expanded by inserting at the end as many elements as needed to reach a size of n.
    ///         If val is specified, the new elements are initialized as copies of val, otherwise,
    ///         they are value-initialized. If n is also greater than the current container capacity, an
    ///         out of range exception is throw.
    /// \param  n The new number of elements.
    void Resize(size_type n)
    {
        CapacityCheck(n);
        (n > m_size) ? ConstructRangeValue(end(), begin() + n)
                     : DestroyRange(begin() + n, end());

        m_size = n;
    }

    /// \brief  See Resize(size_type n).
    /// \param  n The new number of elements.
    /// \value  value The value to use to initialize elements.
    void Resize(size_type n, const_reference value)
    {
        CapacityCheck(n);
        (n > m_size) ? ConstructRangeValue(end(), begin() + n, value)
                     : DestroyRange(begin() + n, end());

        m_size = n;
    }

    /// \brief  Tells if the container is empty.
    /// \return true or false.
    _NODISCARD bool Empty() const _NOEXCEPT
    { return cbegin() == cend(); }

    /// \brief  Returns a reference on the element at the specified index. If index is invalid,
    ///         it is an undefined behavior.
    /// \param  n The index of the element.
    /// \return A reference on the specified element.
    _CONSTEXPR17 reference operator[](size_type n) _NOEXCEPT
    { return *(reinterpret_cast<iterator>(m_alignedStorage) + n); }

    /// \brief  Returns a const reference on the element at the specified index. If index is invalid,
    ///         it is an undefined behavior.
    /// \param  n The index of the element.
    /// \return A const reference on the specified element.
    _CONSTEXPR17 const_reference operator[](size_type n ) const _NOEXCEPT
    { return *(reinterpret_cast<const_iterator>(m_alignedStorage) + n); }

    /// \brief  Returns a reference on the element at the specified index. If index is invalid,
    ///         throws an out of bound exception.
    /// \param  n The index of the element.
    /// \return A reference on the specified element.
    reference At(size_type n)
    {
        RangeCheck(n);
        return (*this)[n];
    }

    /// \brief  Returns a const reference on the element at the specified index. If index is invalid,
    ///         throws an out of bound exception.
    /// \param  n The index of the element.
    /// \return A const reference on the specified element.
    const_reference At(size_type n) const
    {
        RangeCheck(n);
        return (*this)[n];
    }

    /// \brief  Returns a reference on the first element. If the container is empty, it is an undefined behavior.
    /// \return A reference on the first element.
    _CONSTEXPR17 reference Front() _NOEXCEPT
    { return *begin(); }

    /// \brief  Returns a const reference on the first element. If the container is empty, it is an undefined behavior.
    /// \return A const reference on the first element.
    _CONSTEXPR17 const_reference Front() const _NOEXCEPT
    { return *begin(); }

    /// \brief  Returns a reference on the last element. If the container is empty, it is an undefined behavior.
    /// \return A reference on the last element.
    reference Back() _NOEXCEPT
    { return *(end() - 1); }

    /// \brief  Returns a const reference on the last element. If the container is empty, it is an undefined behavior.
    /// \return A const reference on the last element.
    const_reference Back() const _NOEXCEPT
    { return *(end() - 1); }

    /// \brief  Returns a pointer on the internal data.
    /// \return A pointer on the internal data.
    _CONSTEXPR17 pointer Data() _NOEXCEPT
    { return reinterpret_cast<pointer>(m_alignedStorage); }

    /// \brief  Returns a const pointer on the internal data.
    /// \return A const pointer on the internal data.
    _CONSTEXPR17 const_pointer Data() const _NOEXCEPT
    { reinterpret_cast<const_pointer>(m_alignedStorage); }

    /// \brief  Adds (copy) a new element at the end of the container, after its current last element. If the size overflows
    ///         the capacity, throws an out of bound exception.
    /// \param  value The new value to add.
    void PushBack (const_reference value)
    {
        CapacityCheck(m_size);
        Construct(end(), value);
        m_size++;
    }

    /// \brief  Adds (move) a new element at the end of the container, after its current last element. If the size overflows
    ///         the capacity, throws an out of bound exception. This will effectively call EmplaceBack.
    /// \param  value The new value to add (move).
    void PushBack (value_type&& value)
    {
        EmplaceBack(std::move(value));
    }

    /// \brief  Removes the last element. If the container is empty the behavior is undefined.
    void PopBack() _NOEXCEPT
    {
        Destroy(end() - 1);
        m_size--;
    }

    /// \brief  Erases the element at the given position. An invalid position causes an undefined behavior.
    /// \param  position The position of the element to delete.
    /// \return The position of the new element that followed the last element erased.
    iterator Erase (const_iterator position) _NOEXCEPT
    {
        // [position]
        const auto cbeg { cbegin() };
        const auto beg  { begin()  };
        const auto pos  { beg + (position - cbeg) };

        // a b c d e
        // a _ c d e
        // a c d e _
        // a c d e
        Destroy(pos);
        auto it = MoveRange(pos + 1, end(), pos);
        DestroyRange(pos + 1, end());

        m_size--;
        return (beg + (it - cbeg));
    }

    /// \brief  Erases all elements between first and last included. An invalid range causes an undefined behavior.
    /// \param  first The start of the range iterator.
    /// \param  last The end of the range iterator.
    /// \return The position of the new element that followed the last erased range.
    iterator Erase (const_iterator first, const_iterator last) _NOEXCEPT
    {
        // [first, last]
        const auto size = static_cast<size_type>(std::distance(first, last) + 1);
        const auto beg  { begin()  };
        const auto cbeg { cbegin() };
        const auto firstPos { beg + (first - cbeg) + 0};
        const auto lastPos  { beg + (last  - cbeg) + 1};

        // a b c d e
        // a _ _ d e
        // a d e _ _
        // a d e
        DestroyRange(firstPos, lastPos);
        auto it = MoveRange(lastPos, end(), firstPos);
        DestroyRange(lastPos, end());

        m_size -= size;
        return (beg + (it - cbeg));
    }

    /// \brief  Removes all elements from this container.
    void Clear() _NOEXCEPT
    {
        DestroyRange(begin(), end());
        m_size = 0;
    }

    /// \brief  Adds a new element by constructing him in place avoid the copy. If the size overflows
    ///         the capacity, throws an out of bound exception.
    /// \param  args The constructor arguments.
    /// \return The newly added element.
    template <class... Args>
    reference EmplaceBack (Args&&... args)
    {
        CapacityCheck(m_size);
        Construct(end(), std::forward<Args>(args)...);
        m_size++;

        return Back();
    }

    // Note   : Missing insert & emplace implementations
    // Reason : Not required yet.

protected:
    /// \brief  Verifies that the given index is smaller than the current container size.
    ///         If the index is greater or equal, throws an out of range exception.
    /// \param  n The index to verify.
    void RangeCheck(size_type n) const
    {
        if(n >= Size()) {
            throw std::out_of_range("Static Array subscript out of range.");
        }
    }

    /// \brief  Verifies that the given index is smaller than the current container capacity.
    ///         If the index is greater or equal, throws an out of range exception.
    /// \param  n The index to verify.
    void CapacityCheck(size_type n) const
    {
        if(n > Capacity()) {
            throw std::out_of_range("Static Array subscript out of range.");
        }
    }

    /// \brief  Constructs an element at the specified position.
    /// \param  position The position of the element to construct.
    /// \param  args The arguments to use to construct the element.
    template <class InputIterator, class... Args>
    void Construct(InputIterator position, Args&&... args)
    {
#if __cplusplus >= 202000L
        // Only available with a full C++20 compiler
        std::construct_at(std::addressof(*position), std::forward<Args>(args)...);
#else
       // Fall back into the old good placement new
       new (std::addressof(*position)) T(std::forward<Args>(args)...);
#endif
    }

    /// \brief  Constructs a range of elements.
    /// \param  first The start of the range iterator.
    /// \param  last The end of the range iterator.
    /// \param  args The arguments to use to construct the element.
    template <class InputIterator, class... Args>
    void ConstructRangeValue(InputIterator first, InputIterator last, Args&&... args)
    {
        for (; first != last; ++first)
            Construct(first, std::forward<Args>(args)...);
    }

    /// \brief  Constructs a range of elements.
    /// \param  first The start of the range iterator.
    /// \param  last The end of the range iterator.
    /// \param  d_first The start position to construct the range.
    template <class InputIterator, class OutputIterator>
    void ConstructRangeIterator(InputIterator first, InputIterator last, OutputIterator d_first)
    {
        while (first != last)
            Construct(d_first++, *first++);
    }

    /// \brief  Helper to destroy an element by choosing the appropriate way to destruct the element.
    /// \param  position The position iterator of the element to destroy.
    template <class InputIterator>
    void Destroy(InputIterator position) _NOEXCEPT
    {
#if __cplusplus >= 201703L
        // Starting C++17, allocators have no longer the Construct and Destroy methods.
        // They're moved in the allocator_traits interface. But our container doesn't allocate memory
        // and it will be such a waste to store an allocator inside this class.
        std::destroy_at(std::addressof(*position));
#else
        // Before C++17 and without allocator, just call the destructor.
        std::addressof(*position)->~T();
#endif
    }

    /// \brief  Destroys a valid range in the container.
    /// \param  first The start of the range iterator.
    /// \param  last The end of the range iterator.
    template <class InputIterator>
    void DestroyRange(InputIterator first, InputIterator last) _NOEXCEPT
    {
#if __cplusplus >= 201703L
        std::destroy(first, last);
#else
        // Before C++17, use this algorithm which is basically the same of std::destroy
        for(; first != last; ++first)
            Destroy(std::addressof(*first));
#endif
    }

    /// \brief  Moves the specified range at the given position.
    /// \param  first The start of the range to move.
    /// \param  last The end of the range to move
    /// \param  d_first The start position where to move the range.
    template <class InputIterator, class OutputIterator>
    OutputIterator MoveRange(InputIterator first, InputIterator last, OutputIterator d_first)
    {
#if __cplusplus >= 201103L
        return std::move(first, last, d_first);
#else
        while (first != last)
            *d_first++ = std::move(*first++);

        return d_first;
#endif
    }

private:
    /// \brief  Aligned raw byte storage to avoid unaligned data
    ///         and unnecessary constructor calls.
    alignas(Alignment) storage_type m_alignedStorage[N * sizeof(T)];

    /// \brief  The current size of the static array
    size_type m_size { 0 };
};

} // !namespace