#include "editor/gui/fanGuiSlot.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "scene/fanDragnDrop.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"
#include "editor/gui/fanGroupsColors.hpp"



namespace ImGui
{

    //========================================================================================================
    //========================================================================================================
    bool HasCompatibleSlots( const std::vector<fan::SlotBase*>& _slots, const int _argsType )
    {
        for( fan::SlotBase * slot : _slots )
        {
            if( slot->GetArgsType() == _argsType )
            {
                return true;
            }
        }
        return false;
    }

    //========================================================================================================
    //========================================================================================================
    void FanPopupSetSingletonSlot::Open()
    {
        ImGui::OpenPopup( sName );
    }

    //========================================================================================================
    //========================================================================================================
    void FanPopupSetSingletonSlot::Draw( fan::EcsWorld& _world , fan::SlotPtr& _slotPtr )
    {
        const fan::EditorGuiInfo& gui = _world.GetSingleton<fan::EditorGuiInfo>();

        ImGui::SetNextWindowSize( { 400, 400 } );
        if( ImGui::BeginPopupModal( sName,
                                    NULL,
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ) )
        {
            if( ImGui::BeginChild( "child_area", { 370, 330 }, true ) )
            {
                bool didNotDrawAnything = true;
                std::vector<fan::EcsSingletonInfo> infos = _world.GetVectorSingletonInfo();
                for( fan::EcsSingletonInfo& info : infos )
                {
                    if( !HasCompatibleSlots( info.mSlots, _slotPtr.GetArgsType() ) ){ continue; }

                    didNotDrawAnything = false;
                    // display the slot
                    const fan::GuiSingletonInfo& guiInfo = gui.GetSingletonInfo( info.mType );
                    ImGui::Icon( guiInfo.mIcon, { 16, 16 }, fan::GroupsColors::GetColor( guiInfo.mGroup ) );
                    ImGui::SameLine();
                    if( ImGui::TreeNode( info.mName.c_str() ) )
                    {
                        ImGui::Indent();
                        for( fan::SlotBase* slot : info.mSlots )
                        {
                            if( slot->GetArgsType() != _slotPtr.GetArgsType() ){ continue; }
                            if( ImGui::Selectable( slot->mName.c_str() ) )
                            {
                                _slotPtr.SetSingletonSlot( info.mType, slot );
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::Unindent();
                        ImGui::TreePop();
                    }
                }
                if( didNotDrawAnything )
                {
                    ImGui::Text("no compatible slot available");
                }
                ImGui::EndChild();
            }
            if( ImGui::Button( "cancel" ) )
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void FanPopupSetComponentSlot::Open()
    {
        ImGui::OpenPopup( sName );
    }

    //========================================================================================================
    //========================================================================================================
    void FanPopupSetComponentSlot::Draw( fan::EcsWorld& _world , fan::SlotPtr& _slotPtr )
    {
        ImGui::SetNextWindowSize( { 400, 400 } );
        if( ImGui::BeginPopupModal( sName,
                                    NULL,
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ) )
        {
            if( ! _slotPtr.IsComponentSlot() )
            {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return;
            }

            if( ImGui::BeginChild( "child_area", { 370, 330 }, true ) )
            {
                std::vector<fan::EcsComponentInfo*> infos;

                fan::EcsEntity entity = _world.GetEntity( _slotPtr.Data().mHandle );
                bool didNotDrawAnything = true;
                for( int componentIndex = 0; componentIndex < _world.NumComponents(); ++componentIndex )
                {
                    if( !_world.IndexedHasComponent( entity, componentIndex ) ){ continue; }
                    const fan::EcsComponentInfo& info = _world.IndexedGetComponentInfo( componentIndex );
                    if( !HasCompatibleSlots( info.mSlots, _slotPtr.GetArgsType() ) ){ continue; }

                    didNotDrawAnything = false;

                    const fan::EditorGuiInfo& gui = _world.GetSingleton<fan::EditorGuiInfo>();
                    const fan::GuiComponentInfo& guiInfo = gui.GetComponentInfo( info.mType );
                    ImGui::Icon( guiInfo.mIcon, { 16, 16 }, fan::GroupsColors::GetColor( guiInfo.mGroup ) );
                    ImGui::SameLine();
                    if( ImGui::TreeNode( info.mName.c_str() ) )
                    {
                        ImGui::Indent();
                        for( fan::SlotBase* slot : info.mSlots )
                        {
                            if( slot->GetArgsType() != _slotPtr.GetArgsType() ){ continue; }
                            if( ImGui::Selectable( slot->mName.c_str() ) )
                            {
                                const fan::EcsHandle handle = _slotPtr.Data().mHandle;
                                _slotPtr.SetComponentSlot( handle, info.mType, slot );
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::Unindent();
                        ImGui::TreePop();
                    }
                }
                if( didNotDrawAnything )
                {
                    ImGui::Text("no compatible slot available");
                }
                ImGui::EndChild();
            }
            if( ImGui::Button( "cancel" ) )
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

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