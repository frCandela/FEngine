#include "network/components/fanEntityReplication.hpp"
#include "editor/fanGuiInfos.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEntityReplication
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiEntityReplication::OnGui;
            info.mEditorName = "replication";
            info.mEditorPath = "network/";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ImGui::Indent();
            ImGui::Indent();
            {
                EntityReplication& replication = static_cast<EntityReplication&>( _component );
                ImGui::Text( "exclude: %u", replication.mExclude );
                ImGui::Text( "replicated components: " );
                ImGui::Indent();
                const EditorSettings& settings = _world.GetSingleton<EditorSettings>();
                for( uint32_t type : replication.mComponentTypes )
                {
                    const GuiComponentInfo& guiInfo = settings.GetComponentInfo( type );
                    ImGui::Icon( guiInfo.mIcon, { 16, 16 } );
                    ImGui::SameLine();
                    ImGui::Text( "%s", guiInfo.mEditorName.c_str() );
                }
                ImGui::Unindent();
            }
            ImGui::Unindent();
            ImGui::Unindent();
        }
    };
}