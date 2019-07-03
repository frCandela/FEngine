#pragma once

#include <type_traits>

namespace util {
	template<typename T>
	constexpr size_t TypeHash() { return std::type_info(T).hash_code(); }

}