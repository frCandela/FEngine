#include "ecs/gui/fanGuiSlotPtr.hpp"
#include <sstream>

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
                    ImGui::Icon( info.icon, { 16, 16 }, fan::GroupsColors::GetColor( info.group ) );
                    ImGui::SameLine();
                    if( ImGui::TreeNode( info.name.c_str() ) )
                    {
                        ImGui::Indent();
                        for( fan::SlotBase* slot : info.mSlots )
                        {
                            if( slot->GetArgsType() != _slotPtr.GetArgsType() ){ continue; }
                            if( ImGui::Selectable( slot->mName.c_str() ) )
                            {
                                _slotPtr.SetSingletonSlot( info.type, slot );
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
                    ImGui::Icon( info.icon, { 16, 16 }, fan::GroupsColors::GetColor( info.group ) );
                    ImGui::SameLine();
                    if( ImGui::TreeNode( info.name.c_str() ) )
                    {
                        ImGui::Indent();
                        for( fan::SlotBase* slot : info.mSlots )
                        {
                            if( slot->GetArgsType() != _slotPtr.GetArgsType() ){ continue; }
                            if( ImGui::Selectable( slot->mName.c_str() ) )
                            {
                                const fan::EcsHandle handle = _slotPtr.Data().mHandle;
                                _slotPtr.SetComponentSlot( handle, info.type, slot );
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
}