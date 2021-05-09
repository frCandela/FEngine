#pragma once

#include "engine/singletons/fanScene.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiScene
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Scene16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiScene::OnGui;
            info.mEditorName = "scene";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsSingleton& _component )
        {
            Scene& scene = static_cast<Scene&>( _component );
            ImGui::Text( "path: %s", scene.mPath.c_str() );

            if( ImGui::CollapsingHeader( "nodes" ) )
            {
                for( EcsHandle handle : scene.mNodes )
                {
                    SceneNode& sceneNode = _world.GetComponent<SceneNode>( _world.GetEntity( handle ) );
                    ImGui::Text( "%s : %d", sceneNode.mName.c_str(), sceneNode.mHandle );
                }
            }
        }
    };
}