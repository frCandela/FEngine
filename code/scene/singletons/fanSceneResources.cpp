#include "scene/singletons/fanSceneResources.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        SceneResources& sceneResources = static_cast<SceneResources&>( _singleton );
        (void)sceneResources;
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetPointers( PrefabManager* _prefabManager )
    {
        mPrefabManager   = _prefabManager;
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetupResources( PrefabManager& _prefabManager )
    {
        ResourcePtr< Prefab >::sOnResolve.Connect ( &PrefabManager::ResolvePtr, &_prefabManager );
    }
}
