#pragma once

#include "core/memory/fanSerializable.hpp"

namespace fan
{
    class SlotPtr;
    struct ComponentPtrBase;
    struct ResourcePtrData;
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    namespace Serializable
    {
        bool LoadComponentPtr( const Json& _json, const char* _name, ComponentPtrBase& _outPtr );
        void SaveComponentPtr( Json& _json, const char* _name, const ComponentPtrBase& _ptr );

        void SaveSlotPtr( Json& _json, const char* _name, const SlotPtr& _ptr );
        bool LoadSlotPtr( const Json& _json, const char* _name, SlotPtr& _outPtr );
    }
}