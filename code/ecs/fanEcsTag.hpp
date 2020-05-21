#pragma once

#include "core/fanHash.hpp"

namespace fan
{
	//================================
	//================================
	#define ECS_TAG( _TagType )											\
	public:																\
		static constexpr const char* s_name		{ #_TagType		  };	\
		static constexpr uint32_t	 s_type		{ SSID( #_TagType ) };	

	struct EcsTag {};
}