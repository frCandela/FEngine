#pragma once

#include "core/ecs/fanSlot.hpp"

namespace ImGui
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct FanPopupSetSingletonSlot
    {
        static constexpr const char* sName = "Select singleton and slot";
        static void Open();
        static void Draw( fan::EcsWorld& _world, fan::SlotPtr& _slotPtr );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct FanPopupSetComponentSlot
    {
        static constexpr const char* sName = "Select component and slot";

        static void Open();
        static void Draw( fan::EcsWorld& _world, fan::SlotPtr& _slotPtr );
    };

    void FanSlotPtr( const char* _label, fan::EcsWorld& _world, fan::SlotPtr& _ptr );
}