#pragma once

#include <vector>
#include <utility>
#include <cassert>
#include <iterator>

typedef void * key_t;
constexpr key_t null_key_t = nullptr;

// A KeyList is a collection of double chained elements (Keys) that are accessed using key_t handles. Lookup, insertion and deletion have a constant complexity
template<typename Type>
class KeyList
{
public:
	struct Key
	{
		friend class KeyList;

		Key(Type newData) : data(newData) {}

	private:
		Type data;
		Key* prev = nullptr;
		Key* next = nullptr;
	};

	// Insert data at the end of the list and returns it's corresponding key.
	key_t Insert(Type data)
	{
		Key* key = new Key(data);
		key->prev = last;

		if (size == 0)
			first = key;
		else
			last->next = key;
		last = key;
		++size;

		return key;
	}

	// Removes the data associated with the key given in parameter. The key given in parameter becomes invalid
	void Remove(key_t vkey)
	{
		assert(vkey != null_key_t);

		Key* key = static_cast<Key*>(vkey);

		if (size == 1)
		{
			first = nullptr;
			last = nullptr;
		}
		else if (key == first)
		{
			first = key->next;
			first->prev = nullptr;
		}
		else if (key == last)
		{
			last = key->prev;
			last->next = nullptr;
		}
		else
		{
			key->prev->next = key->next;
			key->next->prev = key->prev;
		}
		delete(key);
		--size;
	}

	// Converts the KeyList into a std::vector (copy)
	std::vector<Type> ToVector() const
	{
		std::vector<Type> vec;
		vec.reserve(size);
		Key* it = first;
		while (it)
		{
			vec.push_back(it->data);
			it = it->next;
		}
		return vec;
	}

	// Return a reference on the data associated with the key
	Type& Get(key_t key)
	{
		return static_cast<Key*>(key)->data;
	}

	// Returns the size of the KeyList
	size_t Size() const
	{
		return size;
	}

	// Print the KeyList on std::out.
	void ToString()
	{
		Key* it = first;
		std::cout << "List" << size << ": ";
		while (it)
		{
			if (it == first)
				std::cout << "f-";
			if (it == last)
				std::cout << "l-";
			std::cout << it->data << " ";
			it = it->next;
		}
		std::cout << std::endl;
	}

private:
	Key* first = nullptr;
	Key* last = nullptr;
	size_t size = 0;
};