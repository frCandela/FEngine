#pragma once

#include <set>
#include "core/fanSignal.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "engine/fanEngineSerializable.hpp"
#include "engine/components/fanPrefabInstance.hpp"

namespace fan
{
    class EcsWorld;
    struct SceneNode;
    class Prefab;
    class Resources;

    //==================================================================================================================================================================================================
    // contains the scene tree root and a map of scene nodes for fast access
    // also controls the unique ids for the nodes & the scene serialization to json
    //==================================================================================================================================================================================================
    struct Scene : public EcsSingleton
    {
    ECS_SINGLETON( Scene )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        SceneNode& CreateSceneNode( const std::string _name, SceneNode* const _parentNode, EcsHandle _handle = 0, int _childIndex = -1 );

        void New();
        void Save() const;
        bool LoadFrom( const std::string _path );
        void Clear();
        void SetMainCamera( const EcsHandle _cameraHandle );

        struct ChildPrefab
        {
            PrefabInstance mPrefabInstance;
            SceneNode* mParent;
            int mChildIndex;
        };
        static SceneNode* RLoadFromJson( const Json& _json, Scene& _scene, SceneNode* _parent, const uint32_t _handleOffset, std::vector<ChildPrefab>& _prefabs, int _childIndex = -1 );
        static EcsHandle RFindMaximumHandle( SceneNode& _node );
        static void RSaveToJson( const SceneNode& _node, Json& _json, bool _inlinePrefabs = false );

        using RemapTable = std::map<EcsHandle, EcsHandle>;
        static void GenerateRemapTable( Json& _jsonRootSceneNode, RemapTable& _outRemapTable );
        static void RemapHandlesRecursively( Json& _json, const RemapTable& _remapTable );
        static void BuildResourceList( Resources& _resources, const Json& _json, Json& _outJson );
        static void LoadResourceList( Resources& _resources, const Json& jResources );

        Signal<Scene&>     mOnClear;
        Signal<Scene&>     mOnLoad;
        Signal<SceneNode*> mOnDeleteSceneNode;

        EcsWorld* const mWorld = nullptr;
        std::string         mPath;
        EcsHandle           mRootNodeHandle;
        EcsHandle           mMainCameraHandle;
        std::set<EcsHandle> mNodes;

        SceneNode& GetRootNode() const;
    };
}
