#pragma once

#include "core/fanHash.hpp"

namespace fan
{
	//================================
	//================================
	#define ECS_TAG( _TagType )										\
	public:															\
	template <class T> struct EcsComponentInfoImpl					\
	{																\
		static constexpr const char* s_name{ #_TagType };			\
		static constexpr uint32_t	 s_type{ SSID( #_TagType ) };	\
	};																\
		using Info = EcsComponentInfoImpl< _TagType >;				

	struct EcsTag {};
}