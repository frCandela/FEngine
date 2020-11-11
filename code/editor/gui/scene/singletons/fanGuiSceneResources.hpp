#prama once

#include "scene/singletons/fanSceneResources.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon  = ImGui::IconType::Scene16;
        _info.mGroup = EngineGroups::SceneRender;
        _info.mName  = "scene resources";
        _info.onGui = &SceneResources::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        if( ImGui::CollapsingHeader( "prefabs" ) )
        {
            SceneResources& sceneResources = static_cast<SceneResources&>( _singleton );
            const std::map<std::string, Prefab*>& prefabs = sceneResources.mPrefabManager->GetPrefabs();
            for( auto pair : prefabs )
            {
                ImGui::Text( pair.second->mPath.c_str() );
            }
        }
    }
}
