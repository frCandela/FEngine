#pragma once

#include <set>
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/ecs/fanEcsTypes.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    class EcsWorld;
    struct SceneNode;
    struct EcsComponentInfo;

    //==================================================================================================================================================================================================
    // displays a scene node and its components
    //==================================================================================================================================================================================================
    struct InspectorWindow : EcsSingleton
    {
    ECS_SINGLETON( InspectorWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        static void NewComponentPopup( EcsWorld& _world );
        static std::string SplitPaths( std::vector<std::string>& _current, std::vector<std::string>& _next );
        static void NewComponentItem( EcsWorld& _world, const EcsComponentInfo& _info );
        static void R_DrawHierarchy( std::vector<std::string> _current,
                                     std::string _fullPath,
                                     EcsWorld& _world,
                                     const std::vector<EcsComponentInfo>& _components,
                                     const std::vector<std::string>& _componentsPath );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiInspectorWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "inspector";
            info.mIcon       = ImGui::Inspector16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiInspectorWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}