#include "network/components/fanClientRollback.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiClientRollback
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiClientRollback::OnGui;
            info.mEditorName = "client rollback";
            info.mEditorPath = "network/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ImGui::Indent();
            ImGui::Indent();
            {
                ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );

                const FrameIndex newestFrameIndex = clientRollback.mRollbackDatas.empty()
                        ? 0
                        : clientRollback.mRollbackDatas.back().mFrameIndex;
                const FrameIndex oldestFrameIndex = clientRollback.mRollbackDatas.empty()
                        ? 0
                        : clientRollback.mRollbackDatas.front().mFrameIndex;

                ImGui::Text( "Num saved states: %d", (int)clientRollback.mRollbackDatas.size() );
                ImGui::Text( "Range [%d,%d]", oldestFrameIndex, newestFrameIndex );
            }
            ImGui::Unindent();
            ImGui::Unindent();
        }
    };
}