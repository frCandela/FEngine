#pragma once

#include "core/resources/fanResourcePtr.hpp"
#include "editor/fanResourceInfos.hpp"

//======================================================================================================================================================================================================
//======================================================================================================================================================================================================
namespace ImGui
{
    //=================================================================================================
    //=================================================================================================
    namespace impl
    {
        bool FanResourcePtr( const char* _label, fan::ResourcePtrData& _resourcePtrData, const fan::EditorResourceInfo& _info, const uint32_t _resourceType );
    }

    //=================================================================================================
    //=================================================================================================
    template< class _ResourceType >
    bool FanResourcePtr( const char* _label, fan::ResourcePtrData& _resourcePtrData )
    {
        static_assert( std::is_base_of<fan::Resource, _ResourceType>::value );
        const fan::EditorResourceInfo& resourceInfo = fan::EditorResourceInfo::sResourceInfos.at( _ResourceType::Info::sType );
        return impl::FanResourcePtr( _label, _resourcePtrData, resourceInfo, _ResourceType::Info::sType );
    }
}