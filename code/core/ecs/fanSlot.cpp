#include "fanSlot.hpp"
#include "fanEcsWorld.hpp"
#include "editor/gui/fanGuiSlotPtr.hpp"
#include "scene/fanDragnDrop.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

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
    void SlotPtr::Clear()
    {
        mCallData->mHandle        = 0;
        mCallData->mType = 0;
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
    void SlotPtr::SetComponentSlot( EcsHandle _handle, uint32_t _componentType, SlotBase* _slot )
    {
        fanAssert( mWorld->SafeGetComponentInfo( _componentType ) != nullptr );
        mCallData->mHandle = _handle;
        mCallData->mType = _componentType;
        mCallData->mSlot = _slot;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetComponentSlot( EcsHandle _handle, uint32_t _componentType, const std::string& _slotName )
    {
        SlotBase* slot = nullptr;
        if( _componentType != 0 && !_slotName.empty() )
        {
            const EcsComponentInfo* info = mWorld->SafeGetComponentInfo( _componentType );
            if( info != nullptr )
            {
                for( SlotBase* slotBase : info->mSlots )
                {
                    if( slotBase->mName == _slotName )
                    {
                        slot = slotBase;
                        break;
                    }
                }
            }
        }
        SetComponentSlot( _handle, _componentType, slot );
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetSingletonSlot( uint32_t _singletonType, SlotBase* _slot )
    {
        fanAssert( _singletonType == 0 || mWorld->SafeGetSingletonInfo( _singletonType ) != nullptr );
        mCallData->mHandle = 0;
        mCallData->mType = _singletonType;
        mCallData->mSlot = _slot;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetSingletonSlot( uint32_t _singletonType, const std::string& _slotName )
    {
        SlotBase* slot = nullptr;
        if( _singletonType != 0 && !_slotName.empty() )
        {
            const EcsSingletonInfo* info = mWorld->SafeGetSingletonInfo( _singletonType );
            if( info != nullptr )
            {
                for( SlotBase* slotBase : info->mSlots )
                {
                    if( slotBase->mName == _slotName )
                    {
                        slot = slotBase;
                        break;
                    }
                }
            }
        }
        SetSingletonSlot( _singletonType, slot );
    }
}

namespace ImGui
{
    //========================================================================================================
    //========================================================================================================
    void DrawTooltipSingleton( fan::EcsWorld& _world, fan::SlotPtr& _ptr )
    {
        const fan::EditorGuiInfo& guiInfo = _world.GetSingleton<fan::EditorGuiInfo>();

        ImGui::BeginTooltip();
        const fan::GuiSingletonInfo& targetSingletonInfo = guiInfo.GetSingletonInfo( _ptr.Data().mType );
        ImGui::Icon( targetSingletonInfo.mIcon,
                     { 16, 16 },
                     fan::GroupsColors::GetColor( targetSingletonInfo.mGroup ) );
        ImGui::SameLine();
        ImGui::Text( "target singleton: %s", targetSingletonInfo.mEditorName.c_str() );
        const std::string targetSlotName = _ptr.Data().mSlot == nullptr ?
                "null :" :
                _ptr.Data().mSlot->mName;
        ImGui::Icon( ImGui::IconType::SignalSlot16, { 16, 16 } );
        ImGui::SameLine();
        ImGui::Text( "target slot     : %s", targetSlotName.c_str() );
        ImGui::EndTooltip();
    }

    //========================================================================================================
    //========================================================================================================
    void DrawTooltipComponent( fan::EcsWorld& _world, fan::SlotPtr& _ptr )
    {
        const fan::EditorGuiInfo& guiInfo = _world.GetSingleton<fan::EditorGuiInfo>();

        ImGui::BeginTooltip();
        fan::EcsEntity entity = _world.GetEntity( _ptr.Data().mHandle );
        const fan::GuiComponentInfo& sceneNodeInfo = guiInfo.GetComponentInfo( fan::SceneNode::Info::sType );
        ImGui::Icon( sceneNodeInfo.mIcon,
                     { 16, 16 },
                     fan::GroupsColors::GetColor( sceneNodeInfo.mGroup ) );
        const fan::SceneNode& sceneNode = _world.GetComponent<fan::SceneNode>( entity );
        ImGui::SameLine();
        ImGui::Text( "scene node      : %s", sceneNode.mName.c_str() );

        const fan::GuiComponentInfo& targetComponentInfo = guiInfo.GetComponentInfo( _ptr.Data().mType );
        ImGui::Icon( targetComponentInfo.mIcon,
                     { 16, 16 },
                     fan::GroupsColors::GetColor( targetComponentInfo.mGroup ) );
        ImGui::SameLine();
        ImGui::Text( "target component: %s", targetComponentInfo.mEditorName.c_str() );
        const std::string targetSlotName = _ptr.Data().mSlot == nullptr ?
                "null :" :
                _ptr.Data().mSlot->mName;
        ImGui::Icon( ImGui::IconType::SignalSlot16, { 16, 16 } );
        ImGui::SameLine();
        ImGui::Text( "target slot     : %s", targetSlotName.c_str() );
        ImGui::EndTooltip();
    }

    //========================================================================================================
    //========================================================================================================
    void FanSlotPtr( const char* _label, fan::EcsWorld& _world, fan::SlotPtr& _ptr )
    {
        if( ImGui::ButtonIcon( ImGui::IconType::SignalSlot16, { 16, 16 } ) )
        {
            _ptr.Clear();
        }
        ImGui::FanToolTip( "clear slot" );

        ImGui::SameLine();
        const float width = 0.6f * ImGui::GetWindowWidth() - 16;

        std::string text = "null";
        if(_ptr.IsValid() )
        {
            if( _ptr.IsSingletonSlot() )
            {
                fan::EcsSingletonInfo info = _world.GetSingletonInfo( _ptr.Data().mType );
                const std::string slotName = _ptr.Data().mSlot == nullptr ? "null" : _ptr.Data().mSlot->mName;
                text = info.mName + "::" + slotName;
            }
            else
            {
                fanAssert( _ptr.IsComponentSlot() );
                fan::EcsComponentInfo info = _world.GetComponentInfo( _ptr.Data().mType );
                const std::string slotName = _ptr.Data().mSlot == nullptr ? "null" : _ptr.Data().mSlot->mName;

                text = info.mName + "::" + slotName;
            }
        }

        if( ImGui::Button( text.c_str(), ImVec2( width, 0.f ) ) )
        {
            if( _ptr.IsComponentSlot() ){ FanPopupSetComponentSlot::Open(); }
            else                        { FanPopupSetSingletonSlot::Open(); }
        }
        if( ImGui::IsItemClicked( ImGuiMouseButton_Right ) ){ _ptr.Clear(); }
        if( ImGui::IsItemHovered() )
        {
            if( _ptr.IsSingletonSlot() )        { DrawTooltipSingleton( _world, _ptr ); }
            else if( _ptr.IsComponentSlot() )   { DrawTooltipComponent( _world, _ptr ); }
        }

        ComponentPayload payload = ImGui::FanBeginDragDropTargetComponent<fan::SceneNode>( _world );
        if( payload.IsValid() )
        {
            _ptr.SetComponentSlot( payload.mHandle, payload.mComponentType, nullptr );
            FanPopupSetComponentSlot::Open();
        }

        ImGui::SameLine();
        ImGui::Text( _label );
        ImGui::SameLine();
        ImGui::FanShowHelpMarker("drag a scene node to add a component slot");

        FanPopupSetSingletonSlot::Draw( _world, _ptr );
        FanPopupSetComponentSlot::Draw( _world, _ptr );
    }
}