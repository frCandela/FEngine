#pragma once

#include "scene/singletons/fanSceneResources.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
    struct GuiSceneResources
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::IconType::Scene16;
            info.mGroup = EngineGroups::SceneRender;
            info.onGui  = &GuiSceneResources::OnGui;
            info.mEditorName  = "scene resources";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            if( ImGui::CollapsingHeader( "prefabs" ) )
            {
                SceneResources         & sceneResources = static_cast<SceneResources&>( _singleton );
                const std::map<std::string,
                               Prefab*>& prefabs        = sceneResources.mPrefabManager->GetPrefabs();
                for( auto pair : prefabs )
                {
                    ImGui::Text( pair.second->mPath.c_str() );
                }
            }
        }
    };
}
