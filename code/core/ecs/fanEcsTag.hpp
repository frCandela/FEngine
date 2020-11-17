#pragma once

#include "core/fanHash.hpp"

namespace fan
{
	//========================================================================================================
    //========================================================================================================
	#define ECS_TAG( _TagType )										\
	public:															\
	template <class T> struct EcsComponentInfoImpl					\
	{																\
		static constexpr const char* sName{ #_TagType };			\
		static constexpr uint32_t	 sType{ SSID( #_TagType ) };	\
	};																\
	using Info = EcsComponentInfoImpl< _TagType >;

	struct EcsTag {};

    //========================================================================================================
    //========================================================================================================
    struct EcsTagInfo
    {
        std::string mName;
    };
}