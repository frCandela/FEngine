#pragma once

#include "network/components/fanHostPersistentHandle.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiHostPersistentHandle
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiHostPersistentHandle::OnGui;
            info.mEditorName = "network handle";
            info.mEditorPath = "network/";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            ImGui::Indent();
            ImGui::Indent();
            {
                HostPersistentHandle& networkHandle = static_cast<HostPersistentHandle&>( _component );
                ImGui::Text( "network handle : %d", networkHandle.mHandle );
            }
            ImGui::Unindent();
            ImGui::Unindent();
        }
    };
}