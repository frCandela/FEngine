#pragma once

#include "engine/components/fanExpirationTime.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiExpirationTime
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
             info.mIcon       = ImGui::IconType::Expiration16;
             info.mGroup      = EngineGroups::Scene;
             info.onGui       = &GuiExpirationTime::OnGui;
             info.mEditorPath = "/";
             info.mEditorName       = "expiration time";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ExpirationTime& expiration = static_cast<ExpirationTime&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat( "duration##ExpirationTime", &expiration.mDuration, 0.1f, 0.f, 10.f );
            }
            ImGui::PopItemWidth();
        }
    };
}