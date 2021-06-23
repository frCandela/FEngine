#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/fanSignal.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    struct Scene;
    struct SceneNode;

    //==================================================================================================================================================================================================
    // shows the scene nodes tree
    //==================================================================================================================================================================================================
    struct SceneWindow : EcsSingleton
    {
    ECS_SINGLETON( SceneWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        Signal<SceneNode*> onSelectSceneNode;
        std::string        mPathBuffer;
        char               mTextBuffer[32];
        SceneNode* mLastSceneNodeRightClicked = nullptr;
        bool mExpandSceneHierarchy = false;

        void OnExpandHierarchy( Scene& /*_scene*/ ) { mExpandSceneHierarchy = true; }
        void NewSceneNodeModal( EcsWorld& _world );
        void RenameSceneNodeModal();
        void ExportPrefabModal( EcsWorld& _world );
        void ImportPrefabModal( EcsWorld& _world );
        void PopupRightClick( EcsWorld& _world );
        void R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSceneWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "scene";
            info.mIcon       = ImGui::Scene16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiSceneWindow::OnGui;
            return info;
        }
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}
