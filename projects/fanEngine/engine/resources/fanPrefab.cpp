#include "fanPrefab.hpp"

#include <fstream>
#include "core/fanDebug.hpp"
#include "core/fanPath.hpp"
#include "core/resources/fanResources.hpp"
#include "engine/components/fanPrefabInstance.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanScenePointers.hpp"
#include "engine/singletons/fanApplication.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Prefab::CreateFromJson( const Json& _json )
    {
        Clear();

        if( _json.contains( "prefab" ) )
        {
            mJson = _json;
            return true;
        }
        else
        {
            Debug::Warning() << "Prefab : input is not a prefab" << Debug::Endl();
            return false;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Prefab::CreateFromFile( const std::string& _path )
    {
        Clear();

        std::ifstream inStream( Path::Normalize( _path ) );
        if( inStream.is_open() && inStream.good() )
        {
            Debug::Log() << "loading prefab: " << _path << Debug::Endl();
            inStream >> mJson;

            if( mJson.contains( "prefab" ) )
            {
                mPath = _path;
                return true;
            }
            else
            {
                mJson = Json();
                Debug::Warning() << "file is not a prefab: " << _path << Debug::Endl();
                return false;
            }
        }
        else
        {
            Debug::Warning() << "Failed to open prefab " << _path << Debug::Endl();
            return false;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Prefab::CreateFromSceneNode( const SceneNode& _node )
    {
        Clear();

        Json& prefabJson = mJson["prefab"];
        Scene::RSaveToJson( _node, prefabJson, true );
        Scene::RemapTable remapTable;
        Scene::GenerateRemapTable( prefabJson, remapTable );
        Scene::RemapHandlesRecursively( prefabJson, remapTable );

        // save resources
        EcsWorld & world     = *_node.mScene->mWorld;
        Resources& resources = *world.GetSingleton<Application>().mResources;
        Scene::BuildResourceList( resources, prefabJson, prefabJson );

        EcsEntity entity = world.GetEntity( _node.mHandle );
        if( world.HasComponent<PrefabInstance>( entity ) )
        {
            PrefabInstance& prefabInstance = world.GetComponent<PrefabInstance>( entity );
            if( prefabInstance.mPrefab != nullptr )
            {
                prefabJson["path"] = prefabInstance.mPrefab->mPath;
                prefabJson["guid"] = prefabInstance.mPrefab->mGUID;
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Prefab::Save( const std::string& _path )
    {
        Debug::Log() << Debug::Type::Resources << "Exporting prefab to " << _path << Debug::Endl();
        std::ofstream outStream( Path::Normalize( _path ) );
        if( outStream.is_open() )
        {
            outStream << mJson;
            outStream.close();
            return true;
        }
        Debug::Warning() << "Prefab export failed : " << _path << Debug::Endl();
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SceneNode* Prefab::Instantiate( SceneNode& _parent, const int _childIndex ) const
    {
        if( IsEmpty() )
        {
            Debug::Warning() << "Failed to instantiate prefab : prefab is empty" << Debug::Endl();
            return nullptr;
        }
        else
        {
            const Json& prefabJson = mJson["prefab"];
            EcsWorld  & world      = *_parent.mScene->mWorld;

            // load resources
            Resources& resources = *world.GetSingleton<Application>().mResources;
            Scene::LoadResourceList( resources, prefabJson );

            Scene& scene = world.GetSingleton<Scene>();
            const EcsHandle handleOffset = world.GetNextHandle() - 1;

            std::vector<Scene::ChildPrefab> prefabs;
            SceneNode* newNode = Scene::RLoadFromJson( prefabJson, scene, &_parent, handleOffset, prefabs, _childIndex );
            fanAssert( newNode != nullptr );
            const EcsHandle maxHandle = Scene::RFindMaximumHandle( _parent );
            world.SetNextHandle( maxHandle + 1 );
            ScenePointers::ResolveComponentPointers( world, handleOffset );

            uint32_t    guid = mGUID;
            std::string path = mPath;
            if( mGUID == 0 || mPath.empty() )
            {
                Json::const_iterator jGuid = prefabJson.find( "guid" );
                Json::const_iterator jPath = prefabJson.find( "path" );
                if( jPath != prefabJson.end() && jGuid != prefabJson.end() )
                {
                    guid = *jGuid;
                    path = *jPath;
                }
            }

            if( !path.empty() && guid != 0 )
            {
                ResourcePtr<Prefab> prefabPtr = ResourcePtrData::sResourceManager->Get<Prefab>( guid );
                if( prefabPtr != nullptr )
                {
                    EcsEntity entity = world.GetEntity( newNode->mHandle );
                    PrefabInstance& prefabInstance = world.AddComponent<PrefabInstance>( entity );
                    prefabInstance.mPrefab = prefabPtr;
                }
            }

            return newNode;
        }
    }
}