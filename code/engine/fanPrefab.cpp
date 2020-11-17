#include "engine/fanPrefab.hpp"

#include <fstream>
#include "core/fanDebug.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanScenePointers.hpp"

namespace fan
{
    //========================================================================================================
	//========================================================================================================
	bool Prefab::CreateFromJson( const Json& _json )
	{
		Clear();

		if ( _json.contains( "prefab" ) )
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

	//========================================================================================================
	//========================================================================================================
	bool Prefab::CreateFromFile( const std::string& _path )
	{
		Clear();

		std::ifstream inStream( _path );
		if ( inStream.is_open() && inStream.good() )
		{
			Debug::Get() << Debug::Severity::log << "loading prefab: " << _path << Debug::Endl();
			inStream >> mJson;

			if ( mJson.contains( "prefab" ) )
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
			Debug::Warning() << "Prefab failed to open file " << _path << Debug::Endl();
			return false;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void Prefab::CreateFromSceneNode( const SceneNode& _node )
	{
		Clear();

		Json& prefabJson = mJson[ "prefab" ];
        Scene::RSaveToJson( _node, prefabJson );
        Scene::RemapTable remapTable;
        Scene::GenerateRemapTable( prefabJson, remapTable );
        Scene::RemapHandlesRecursively( prefabJson, remapTable );
	}

	//========================================================================================================
	//========================================================================================================
	SceneNode* Prefab::Instantiate( SceneNode& _parent ) const
	{
		if( IsEmpty() )
		{
			Debug::Warning() << "Failed to instantiate prefab : prefab is empty" << Debug::Endl();
			return nullptr;
		}
		else
		{
			EcsWorld& world = *_parent.mScene->mWorld;
			Scene& scene = world.GetSingleton<Scene>();
			const EcsHandle handleOffset = world.GetNextHandle() - 1;
			SceneNode& newNode = Scene::RLoadFromJson( mJson["prefab"], scene, &_parent, handleOffset );
			const EcsHandle maxHandle = Scene::RFindMaximumHandle( _parent );
			world.SetNextHandle( maxHandle + 1);
			ScenePointers::ResolveComponentPointers( world, handleOffset );
			return &newNode;
		}		
	}
}