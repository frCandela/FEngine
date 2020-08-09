#pragma once

#include "ecs/fanSlot.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace ImGui
{
    //========================================================================================================
    //========================================================================================================
    struct FanGuiSlotPtr
    {
        FanGuiSlotPtr( fan::EcsWorld& _world ) : mWorld( _world ){}
        void GenerateFrom( fan::SlotPtr& _ptr );

        fan::EcsWorld&                            mWorld;
        fan::EcsEntity                            mEntity;
        std::string                               mSceneNodeName;
        int                                       mComponentIndex;
        int                                       mSlotIndex;
        std::string                               mComboComponentsStr;
        std::string                               mComboSlotsStr;
        std::vector<const fan::EcsComponentInfo*> mComponentsInfo;

    private:
        static std::string GetNullString();
        void GenerateComponentInfos( std::vector<const fan::EcsComponentInfo*>& _infos ) const;
        std::string GenerateStrComboComponents() const;
        std::string GenerateStrComboSlots() const;
        int  GetComponentIndex( const fan::SlotPtr& _ptr ) const;
        int  GetSlotIndex( const fan::SlotPtr& _ptr ) const;
    };
}