#include "ecs/fanSlot.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "ecs/gui/fanGuiSlotPtr.hpp"
#include "scene/fanDragnDrop.hpp"
#include "editor/fanModals.hpp"

namespace  fan
{
    //========================================================================================================
    //========================================================================================================
    SlotPtr::SlotPtr()
    {
        mCallData = new SlotCallData();
    }

    //========================================================================================================
    //========================================================================================================
    SlotPtr::~SlotPtr()
    {
        delete mCallData;
        mCallData = nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    bool SlotPtr::IsValid() const
    {
        return mCallData->mHandle != 0 && mCallData->mComponentType != 0;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::Clear()
    {
        mCallData->mHandle        = 0;
        mCallData->mComponentType = 0;
        mCallData->mSlot          = nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::Init( EcsWorld& _world, int _argsType )
    {
        mWorld = &_world;
        mArgsType = _argsType;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::Set( EcsHandle _handle, uint32_t _componentType, SlotBase* _slot )
    {
        mCallData->mHandle        = _handle;
        mCallData->mComponentType = _componentType;
        mCallData->mSlot          = _slot;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::Set( EcsHandle _handle, uint32_t _componentType, const std::string& _slotName )
    {
        SlotBase* slot = nullptr;
        if( _componentType != 0 && !_slotName.empty() )
        {
            const EcsComponentInfo* info = mWorld->SafeGetComponentInfo( _componentType );
            for( SlotBase* slotBase : info->mSlots )
            {
                if( slotBase->mName == _slotName )
                {
                    slot = slotBase;
                    break;
                }
            }
        }

        Set( _handle, _componentType, slot );
    }
}

namespace ImGui
{
    //========================================================================================================
    //========================================================================================================
    bool FanDragDropTargetSlot( fan::EcsWorld& _world, fan::SlotPtr& _slotPtr )
    {
        ImGui::ComponentPayload payload = ImGui::FanBeginDragDropTargetComponent( _world, 0 );
        if( payload.mHandle != 0  && payload.mComponentType != 0)
        {
            const fan::EcsComponentInfo& info = _world.GetComponentInfo( payload.mComponentType );
            fan::SlotBase * slot = info.mSlots.empty() ? nullptr : info.mSlots[0];
            _slotPtr.Set( payload.mHandle, payload.mComponentType, slot );
            return true;
        }
        return false;
    }

    //========================================================================================================
    //========================================================================================================
    void FanSlotPtr( const char* _label, fan::EcsWorld& _world, fan::SlotPtr& _ptr )
    {
        FanGuiSlotPtr guiSlot( _world );
        guiSlot.GenerateFrom( _ptr );

        ImGui::ButtonIcon( ImGui::IconType::SIGNAL_SLOT16, {16,16} );
        ImGui::FanToolTip("signal");
        ImGui::SameLine();

        const float width = 0.2f * ImGui::GetWindowWidth() - 2.f * 5.33f; // 16/3 = 5.33

        ImGui::Button( guiSlot.mSceneNodeName.c_str(), ImVec2( width, 0.f ) );
        ImGui::FanToolTip("target scene node");
        FanDragDropTargetSlot( _world, _ptr );
        if( ImGui::IsItemClicked( ImGuiMouseButton_Right ) ){ _ptr.Clear(); }

        ImGui::SameLine();
        ImGui::PushItemWidth( width );
        if( ImGui::Combo( "##componentType", &guiSlot.mComponentIndex, guiSlot.mComboComponentsStr.c_str() ) )
        {
            if( _ptr.IsValid() && ! guiSlot.mComponentsInfo.empty() )
            {
                const fan::EcsComponentInfo* info = guiSlot.mComponentsInfo[ guiSlot.mComponentIndex ];
                const fan::EcsHandle handle = _ptr.Data().mHandle;
                const uint32_t componentType = info->type;
                fan::SlotBase * slot = info->mSlots.empty() ? nullptr : info->mSlots[0];
                _ptr.Set( handle, componentType, slot );
            }
        }
        ImGui::FanToolTip("target component");
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::PushItemWidth( width );
        if( ImGui::Combo( "##slot", &guiSlot.mSlotIndex, guiSlot.mComboSlotsStr.c_str() ) )
        {
            if( _ptr.IsValid() &&
                !guiSlot.mComponentsInfo.empty() &&
                !guiSlot.mComponentsInfo[guiSlot.mComponentIndex]->mSlots.empty() )
            {
                const fan::EcsHandle handle = _ptr.Data().mHandle;
                const uint32_t componentType = _ptr.Data().mComponentType;
                const fan::EcsComponentInfo* info = guiSlot.mComponentsInfo[ guiSlot.mComponentIndex ];
                fan::SlotBase * slot = info->mSlots[ guiSlot.mSlotIndex ];
                _ptr.Set( handle, componentType, slot );
            }
        }
        ImGui::FanToolTip("target slot");
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Text( _label );
    }
}