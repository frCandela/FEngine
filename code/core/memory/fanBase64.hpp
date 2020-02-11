#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{
	//================================================================================================================================
	// Base64 string encoding and decoding
	//================================================================================================================================
	class Base64
	{
	public:
		static std::string  Encode( const unsigned char* _bytesToEncode, const unsigned int _lenght );
		static std::string  Decode( const std::string& _encodedString );
	};
}