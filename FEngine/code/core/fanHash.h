#pragma once

#include <cstdint>

namespace hash_str
{

	constexpr const int8_t   STRING_END_BYTE = 0;
	constexpr const uint32_t STRING_BIT_SHIT = 5;

	/// \brief   Magic number from string hash (k = 33)
	/// \warning Do not edit
	constexpr const uint32_t STRING_HASH_KEY = 42;

	/// \brief  Computes a 32 bits hash from a c string
	/// \param  pTail A pointer on the c string
	/// \param  hash The result
	/// \return A 32 bits hash
	///
	/// \note   This function is tail recursive
	constexpr uint32_t hash_function(const char* pTail, uint32_t hash = STRING_HASH_KEY)
	{
		return (pTail[0] == STRING_END_BYTE) ? hash :
			hash_function(pTail + 1, ((hash << STRING_BIT_SHIT) + hash) + (int32_t)*pTail);
	}

	/// \brief  Binds the constexpr tail recursive function to a template parameter
	///         to force the compiler to hash the string compile time
	/// \tparam the hash as a template parameter
	/// \return see hash_function
	template <uint32_t hash>
	inline constexpr uint32_t compile_time_hash()
	{
		return hash;
	}

} // !namespace

/// \example DSID
///
/// DSID is the dynamic or run-time version of SSID
/// It takes in parameter a const char *
/// Example : U32 goID_1 = DSID(go.c_str());
///           U32 goID_2 = DSID("BossDoor");
///           U32 goID_3 = DSID(szP)
#define DSID(string) hash_str::hash_function(string)

/// \example SSID
///
/// To pre-hash all strings and get back a 32 bits unsigned int
/// at compile time, you have to call the SSID macro.
/// It takes in parameter a literal const char *
/// Example : uint32_t identifier = SSID("MyLiteralIdentifier");
#define SSID(string) \
__pragma(warning(suppress: 4307)) \
hash_str::compile_time_hash<hash_str::hash_function(string)>()
