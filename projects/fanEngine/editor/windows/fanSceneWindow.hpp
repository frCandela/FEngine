#pragma once

#include "core/ecs/fanSignal.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
    struct Scene;
    struct SceneNode;

    //==================================================================================================================================================================================================
    // shows the scene nodes tree
    //==================================================================================================================================================================================================
    class SceneWindow : public EditorWindow
    {
    public:
        Signal<SceneNode*> onSelectSceneNode;

        SceneWindow();
        ~SceneWindow();

        // Callbacks
        void OnExpandHierarchy( Scene& /*_scene*/ ) { mExpandSceneHierarchy = true; }

    protected:
        void OnGui( EcsWorld& _world ) override;

    private:
        std::string mPathBuffer;
        char        mTextBuffer[32];
        SceneNode* mLastSceneNodeRightClicked = nullptr;
        bool mExpandSceneHierarchy = false;

        void NewSceneNodeModal( EcsWorld& _world );
        void RenameSceneNodeModal();
        void ExportPrefabModal( EcsWorld& _world );
        void ImportPrefabModal( EcsWorld& _world );
        void PopupRightClick( EcsWorld& _world );

        void R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked );
    };
}
