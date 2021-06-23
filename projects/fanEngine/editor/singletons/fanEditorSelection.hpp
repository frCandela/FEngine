#pragma once

#include "ecs/fanSignal.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    struct Scene;
    struct SceneNode;

    //==================================================================================================================================================================================================
    // Manages the editor scene node selection
    //==================================================================================================================================================================================================
    struct EditorSelection : EcsSingleton
    {
    ECS_SINGLETON( EditorSelection )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        void ConnectCallbacks( Scene& _scene );
        void SetSelectedSceneNode( SceneNode* _node );
        void Deselect();
        void DeleteSelection();
        void Update( const bool _gameWindowHovered );

        SceneNode* GetSelectedSceneNode() const;

        EcsHandle mSelectedNodeHandle = 0;
        Scene* mCurrentScene = nullptr;

        void OnSceneNodeDeleted( SceneNode* _node );
        void OnToogleTransformLock();
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorSelection
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor selection";
            info.mIcon       = ImGui::Selection16;
            info.mGroup      = EngineGroups::Editor;
            return info;
        }
    };
}