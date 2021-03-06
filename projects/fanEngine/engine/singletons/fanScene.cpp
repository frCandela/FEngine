#include "engine/singletons/fanScene.hpp"

#include <stack>
#include <fstream>
#include <engine/components/fanPrefabInstance.hpp>
#include <engine/ui/fanUITransform.hpp>
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include "core/resources/fanResources.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScenePointers.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::Init( EcsWorld& _world, EcsSingleton& _component )
    {
        Scene& scene = static_cast<Scene&>( _component );
        scene.mPath             = "";
        scene.mRootNodeHandle   = 0;
        scene.mMainCameraHandle = 0;
        scene.mNodes.clear();
        EcsWorld*& worldNoConst = const_cast<EcsWorld*&>( scene.mWorld );
        worldNoConst = &_world;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SceneNode& Scene::GetRootNode() const
    {
        return mWorld->GetComponent<SceneNode>( mWorld->GetEntity( mRootNodeHandle ) );
    }

    //==================================================================================================================================================================================================
    // _handle can be used to force the handle of scene node entity,
    // if _handle=0 (by default), generate a new handle
    //==================================================================================================================================================================================================
    SceneNode& Scene::CreateSceneNode( const std::string _name, SceneNode* const _parentNode, EcsHandle _handle, int _childIndex )
    {
        fanAssert( _parentNode != nullptr || mRootNodeHandle == 0 ); // we can have only one root node

        EcsEntity entity = mWorld->CreateEntity();

        // set entity handle
        EcsHandle handle = 0;
        if( _handle == 0 )
        {
            handle = mWorld->AddHandle( entity );
        }
        else
        {
            mWorld->SetHandle( entity, _handle );
            handle = _handle;
        }
        fanAssert( mNodes.find( handle ) == mNodes.end() );
        mNodes.insert( handle );

        SceneNode& sceneNode = mWorld->AddComponent<SceneNode>( entity );
        mWorld->AddComponent<Bounds>( entity );
        sceneNode.AddFlag( SceneNode::BoundsOutdated );
        if( _parentNode == nullptr ) // root node
        {
            sceneNode.AddFlag( SceneNode::NoRaycast );
            sceneNode.AddFlag( SceneNode::NoDelete );
        }

        sceneNode.Build( _name, *this, handle, _parentNode, _childIndex );
        return sceneNode;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    EcsHandle Scene::RFindMaximumHandle( SceneNode& _node )
    {
        EcsWorld& world = *_node.mScene->mWorld;

        EcsHandle handle = _node.mHandle;
        const std::vector<EcsHandle>& childs = _node.mChilds;
        for( int childIndex = 0; childIndex < (int)childs.size(); childIndex++ )
        {
            if( childs[childIndex] != 0 ) // can be zero when serializing prefabs
            {
                SceneNode& node = world.GetComponent<SceneNode>( world.GetEntity( childs[childIndex] ) );
                EcsHandle childHandle = RFindMaximumHandle( node );
                if( childHandle > handle )
                {
                    handle = childHandle;
                }
            }
        }
        return handle;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::SetMainCamera( const EcsHandle _cameraHandle )
    {
        if( _cameraHandle != mMainCameraHandle )
        {
            mMainCameraHandle = _cameraHandle;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::Clear()
    {
        mOnClear.Emmit( *this );
        mPath = "";
        mWorld->Clear();
        mNodes.clear();
        mRootNodeHandle   = 0;
        mMainCameraHandle = 0;
        ScenePointers::Clear( mWorld->GetSingleton<ScenePointers>() );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::New()
    {
        Clear();
        mWorld->InitSingletons();
        SceneNode& rootNode = CreateSceneNode( "root", nullptr );
        mRootNodeHandle = rootNode.mHandle;
        mOnLoad.Emmit( *this );
        mWorld->ApplyTransitions();
        mWorld->PostInitSingletons();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::Save() const
    {
        Json json;

        // scene global parameters
        Json& jScene = json["scene"];
        Serializable::SaveStr( jScene, "path", Path::MakeRelative( mPath ) );

        // save singleton components
        Json& jSingletons = jScene["singletons"];
        unsigned nextIndex = 0;
        const std::vector<EcsSingletonInfo>& singletonsInfos = mWorld->GetVectorSingletonInfo();
        std::string binarySavePath = Path::Directory( mPath ) + Path::FileName( mPath );
        for( const EcsSingletonInfo& info : singletonsInfos )
        {
            if( info.save != nullptr )
            {
                Json& jSingleton_i = jSingletons[nextIndex++];
                Serializable::SaveUInt( jSingleton_i, "singleton_id", info.mType );
                Serializable::SaveStr( jSingleton_i, "singleton", info.mName );
                EcsSingleton& singleton = mWorld->GetSingleton( info.mType );
                info.save( singleton, jSingleton_i );
                if( info.saveBinary != nullptr )
                {
                    info.saveBinary( singleton, binarySavePath.c_str() );
                }
            }
        }

        // saves all scene nodes recursively
        Json& jRoot = jScene["root"];
        RSaveToJson( GetRootNode(), jRoot );

        Scene::RemapTable remapTable;
        GenerateRemapTable( jRoot, remapTable );
        RemapHandlesRecursively( jScene, remapTable );

        // save resources
        Resources& resources = *mWorld->GetSingleton<Application>().mResources;
        BuildResourceList( resources, jScene, jScene );

        std::ofstream outStream( Path::Normalize( mPath ) );
        if( outStream.is_open() )
        {
            outStream << json;
            outStream.close();
            Debug::Highlight() << "saved scene: " << mPath << Debug::Endl();
        }
        else
        {
            Debug::Error() << "Failed to save scene: " << mPath << Debug::Endl();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::RSaveToJson( const SceneNode& _node, Json& _json, bool _inlinePrefabs )
    {
        EcsWorld& world = *_node.mScene->mWorld;
        Serializable::SaveStr( _json, "name", _node.mName );
        Serializable::SaveUInt( _json, "handle", _node.mHandle );

        EcsEntity entity = world.GetEntity( _node.mHandle );
        Json& jComponents = _json["components"];
        Json& jchilds     = _json["childs"];
        Json& jTags       = _json["tags"];

        // if the node is a prefab, only save the prefab instance component and an optional transform/ui transform
        if( !_inlinePrefabs && world.HasComponent<PrefabInstance>( entity ) )
        {
            // save resource
            {
                PrefabInstance& prefabInstance = world.GetComponent<PrefabInstance>( entity );
                PrefabInstance::CreateOverride( prefabInstance, world, entity );
                fanAssert( prefabInstance.mPrefab != nullptr );
                const EcsComponentInfo& info         = world.GetComponentInfo( PrefabInstance::Info::sType );
                Json                  & jComponent_i = jComponents[0];
                Serializable::SaveUInt( jComponent_i, "component_type", info.mType );
                Serializable::SaveStr( jComponent_i, "type_name", info.mName );
                PrefabInstance::Save( prefabInstance, jComponent_i );
            }
        }
        else
        {
            // save tags
            {
                unsigned nextIndex = 0;
                for( const EcsTagInfo& info : world.GetTagsInfos() )
                {
                    if( !world.HasTag( entity, info.mType ) ){ continue; }
                    jTags[nextIndex++] = info.mType;
                }
            }

            // save components
            {
                unsigned nextIndex = 0;
                for( const EcsComponentInfo& info : world.GetComponentInfos() )
                {
                    if( !world.HasComponent( entity, info.mType ) ){ continue; }

                    // if a save method is provided, saves the component
                    EcsComponent& component = world.GetComponent( entity, info.mType );
                    if( info.save != nullptr )
                    {
                        Json& jComponent_i = jComponents[nextIndex++];
                        Serializable::SaveUInt( jComponent_i, "component_type", info.mType );
                        Serializable::SaveStr( jComponent_i, "type_name", info.mName );
                        info.save( component, jComponent_i );
                    }
                }
            }

            // save childs
            unsigned childIndex     = 0;
            for( int sceneNodeIndex = 0; sceneNodeIndex < (int)_node.mChilds.size(); sceneNodeIndex++ )
            {
                const EcsHandle childHandle = _node.mChilds[sceneNodeIndex];
                SceneNode& childNode = world.GetComponent<SceneNode>( world.GetEntity( childHandle ) );
                if( !childNode.HasFlag( SceneNode::NoSave ) )
                {
                    Json& jchild = jchilds[childIndex];
                    RSaveToJson( childNode, jchild );
                    ++childIndex;
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    // parse all the json, get all the scene nodes handles & generates a remap table on a range close to zero
    // ex: 400, 401, 1051 will be remapped to 1,2,3
    //==================================================================================================================================================================================================
    void Scene::GenerateRemapTable( Json& _jsonRootSceneNode, RemapTable& _outRemapTable )
    {
        int                     nextRemapIndex = 1;
        std::stack<const Json*> stack;
        stack.push( &_jsonRootSceneNode );
        while( !stack.empty() )
        {
            const Json& jNode = *stack.top();
            stack.pop();

            uint32_t id = jNode["handle"];
            _outRemapTable[id] = nextRemapIndex++;

            // push all childs
            const Json& jchilds = jNode["childs"];
            {
                for( int childIndex = 0; childIndex < (int)jchilds.size(); childIndex++ )
                {
                    const Json& jChild = jchilds[childIndex];
                    stack.push( &jChild );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    // remaps scene nodes handles & component pointers  according to a remap table
    //==================================================================================================================================================================================================
    void Scene::RemapHandlesRecursively( Json& _json, const RemapTable& _remapTable )
    {
        std::stack<Json*> stack;
        stack.push( &_json );
        while( !stack.empty() )
        {
            Json& js = *stack.top();
            stack.pop();

            // remap
            Json::iterator jNodeId = js.find( "handle" );
            if( jNodeId != js.end() )
            {
                const uint32_t nodeId = *jNodeId;
                auto           it     = _remapTable.find( nodeId );
                if( nodeId == 0 ){}
                else if( it != _remapTable.end() )
                {
                    const uint32_t remapId = it->second;
                    *jNodeId = remapId;
                }
                else
                {
                    *jNodeId = 0;// pointer target is outside of the json nodes
                }
            }

            // push all childs
            if( js.is_structured() )
            {
                for( auto& element : js )
                {
                    stack.push( &element );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::BuildResourceList( Resources& _resources, const Json& _json, Json& _outJson )
    {
        std::vector<uint32_t>   resourcesGuids;
        std::stack<const Json*> stack;
        stack.push( &_json );
        while( !stack.empty() )
        {
            const Json& json = *stack.top();
            stack.pop();

            Json::const_iterator jResourceIt = json.find( "resource" );
            if( jResourceIt != json.end() )
            {
                const uint32_t guid = *jResourceIt;
                if( guid != 0 )
                {
                    resourcesGuids.push_back( guid );
                }
            }

            // push all childs
            if( json.is_structured() )
            {
                for( const Json& element : json )
                {
                    stack.push( &element );
                }
            }
        }

        Json& jResources = _outJson["resources"];
        int           resourceIndex = 0;
        for( uint32_t guid : resourcesGuids )
        {
            ResourcePtrData data = _resources.Get( guid );
            if( !data.mHandle->mResource->mIsGenerated )
            {
                fanAssert( data.mHandle != nullptr );
                const Resource* resource = data.mHandle->mResource;
                fanAssert( !Path::IsAbsolute( resource->mPath ) );
                Json& jResource = jResources[resourceIndex++];
                jResource["type"] = resource->mType;
                jResource["path"] = resource->mPath;
                jResource["guid"] = resource->mGUID;
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Scene::LoadResourceList( Resources& _resources, const Json& _json )
    {
        Json::const_iterator it = _json.find( "resources" );
        if( it == _json.end() ){ return; }

        const Json& jResources = *it;
        for( int i = 0; i < (int)jResources.size(); i++ )
        {
            const Json& jResource = jResources[i];
            const uint32_t    type = jResource["type"];
            const std::string path = jResource["path"];
            _resources.GetOrLoad( type, path );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Scene::LoadFrom( const std::string _path )
    {
        Clear();
        std::ifstream inStream( Path::Normalize( _path ) );
        if( inStream.is_open() )
        {
            fanAssert( inStream.good() );
            // Load scene
            Json sceneJson;
            inStream >> sceneJson;
            inStream.close();

            // scene global parameters
            const Json& jScene = sceneJson["scene"];
            {
                Serializable::LoadStr( jScene, "path", mPath );
            }

            //resources
            Resources& resources = *mWorld->GetSingleton<Application>().mResources;
            LoadResourceList( resources, jScene );

            // load singleton components
            mWorld->InitSingletons();
            std::string binaryLoadPath = Path::Directory( _path ) + Path::FileName( _path );
            if( jScene.find( "singletons" ) != jScene.end() )
            {
                const Json& jSingletons = jScene["singletons"];
                for( int childIndex = 0; childIndex < (int)jSingletons.size(); childIndex++ )
                {
                    const Json& jSingleton_i = jSingletons[childIndex];
                    unsigned staticIndex = 0;
                    Serializable::LoadUInt( jSingleton_i, "singleton_id", staticIndex );

                    const EcsSingletonInfo* info = mWorld->SafeGetSingletonInfo( staticIndex );
                    if( info != nullptr && info->load != nullptr )
                    {
                        EcsSingleton& singleton = mWorld->GetSingleton( staticIndex );
                        info->load( singleton, jSingleton_i );
                        if( info->loadBinary != nullptr )
                        {
                            info->loadBinary( singleton, binaryLoadPath.c_str() );
                        }
                    }
                    else
                    {
                        Debug::Error() << "corrupted singleton component with id " << staticIndex << Debug::Endl();
                    }
                }
            }

            // loads all nodes recursively
            const Json& jRoot = jScene["root"];
            const EcsHandle          handleOffset = 0;
            std::vector<ChildPrefab> prefabs;
            SceneNode& rootNode = *RLoadFromJson( jRoot, *this, nullptr, handleOffset, prefabs );
            mRootNodeHandle = rootNode.mHandle;

            mPath = _path;
            const EcsHandle maxHandle = RFindMaximumHandle( rootNode ) + 1;
            mWorld->SetNextHandle( maxHandle );

            // instantiate prefabs
            for( const ChildPrefab& childPrefab : prefabs )
            {
                const PrefabInstance& prefabInstance = childPrefab.mPrefabInstance;
                SceneNode           * prefab         = prefabInstance.mPrefab->Instantiate( *childPrefab.mParent, childPrefab.mChildIndex );
                EcsEntity entity = mWorld->GetEntity( prefab->mHandle );
                PrefabInstance::ApplyOverride( prefabInstance, *mWorld, entity );
            }

            ScenePointers::ResolveComponentPointers( *mWorld, handleOffset );

            mOnLoad.Emmit( *this );
            mWorld->ApplyTransitions();
            mWorld->PostInitSingletons();

            mWorld->Run<SInitFollowTransforms>();

            return true;
        }
        else
        {
            Debug::Error() << "failed to open file " << _path << Debug::Endl();
            New();
            return false;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SceneNode* Scene::RLoadFromJson( const Json& _json, Scene& _scene, SceneNode* _parent, const uint32_t _handleOffset, std::vector<ChildPrefab>& _prefabs, int _childIndex )
    {
        EcsWorld& world = *_scene.mWorld;

        const Json& jComponents = _json["components"];
        const Json& jTags       = _json["tags"];

        // if a PrefabInstance component is present, push in the prefab list and add a null child. Prefab will be instanced later
        if( jComponents.size() == 1 && jComponents[0]["component_type"] == PrefabInstance::Info::sType )
        {
            ChildPrefab childPrefab;
            PrefabInstance::Load( childPrefab.mPrefabInstance, jComponents[0] );
            childPrefab.mParent     = _parent;
            childPrefab.mChildIndex = (int)_parent->mChilds.size();
            _prefabs.push_back( childPrefab );
            _parent->mChilds.push_back( 0 );
            return nullptr;
        }

        // load globals
        EcsHandle nodeHandle;
        Serializable::LoadUInt( _json, "handle", nodeHandle );
        SceneNode& node = _scene.CreateSceneNode( "tmp", _parent, nodeHandle + _handleOffset, _childIndex );
        Serializable::LoadStr( _json, "name", node.mName );
        _scene.mNodes.insert( node.mHandle );

        const EcsEntity entity = world.GetEntity( node.mHandle );

        // load tags
        {
            for( int i = 0; i < jTags.size(); i++ )
            {
                const uint32_t tagType = jTags[i];
                if( world.SafeGetTagInfo( tagType ) )
                {
                    world.AddTag( entity, tagType );
                }
            }
        }

        // load components
        std::vector<const EcsComponentInfo*> postInitList;
        for( int childIndex = 0; childIndex < (int)jComponents.size(); childIndex++ )
        {
            const Json& jComponent_i = jComponents[childIndex];
            unsigned staticIndex = 0;
            Serializable::LoadUInt( jComponent_i, "component_type", staticIndex );

            const EcsComponentInfo* info = world.SafeGetComponentInfo( staticIndex );
            if( info != nullptr )
            {
                EcsComponent& component = world.AddComponent( entity, staticIndex );
                info->load( component, jComponent_i );

                if( info->postInit != nullptr )
                {
                    postInitList.push_back(info);
                }
            }
        }

        // post init components
        for( const EcsComponentInfo* info : postInitList )
        {
            info->postInit( world, entity );
        }

        // Load childs
        const Json& jchilds = _json["childs"];
        {
            for( int childIndex = 0; childIndex < (int)jchilds.size(); childIndex++ )
            {
                const Json& jchild_i = jchilds[childIndex];
                {
                    RLoadFromJson( jchild_i, _scene, &node, _handleOffset, _prefabs );
                }
            }
        }
        return &node;
    }
}