#include "scene/fanPrefab.hpp"

#include <fstream>
#include "core/fanDebug.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanScene.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletons/fanScenePointers.hpp"

namespace fan
{
	PrefabManager Prefab::s_resourceManager;

	//================================================================================================================================
	//================================================================================================================================
	bool Prefab::CreateFromJson( const Json& _json )
	{
		Clear();

		if ( _json.contains( "prefab" ) )
		{
			m_json = _json;
			return true;
		}
		else
		{
			Debug::Warning() << "Prefab : input is not a prefab" << Debug::Endl();
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Prefab::CreateFromFile( const std::string& _path )
	{
		Clear();

		std::ifstream inStream( _path );
		if ( inStream.is_open() && inStream.good() )
		{
			Debug::Get() << Debug::Severity::log << "loading prefab: " << _path << Debug::Endl();
			inStream >> m_json;

			if ( m_json.contains( "prefab" ) )
			{
				m_path = _path;
				return true;
			}
			else
			{
				m_json = Json();
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

	//================================================================================================================================
	//================================================================================================================================
	void Prefab::CreateFromSceneNode( const SceneNode& _node )
	{
		Clear();

		Json& prefabJson = m_json[ "prefab" ];
		Scene::R_SaveToJson( _node, prefabJson );
		Scene::RemapSceneNodesIndices( prefabJson );
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneNode* Prefab::Instanciate( SceneNode& _parent ) const
	{
		if( IsEmpty() )
		{
			Debug::Warning() << "Failed to instantiate prefab : prefab is empty" << Debug::Endl();
			return nullptr;
		}
		else
		{
			EcsWorld& world = *_parent.scene->world;
			Scene& scene = world.GetSingleton<Scene>();
			const EcsHandle handleOffset = world.GetNextHandle() - 1;
			SceneNode& newNode = Scene::R_LoadFromJson( m_json["prefab"], scene, &_parent, handleOffset );
			const EcsHandle maxHandle = Scene::R_FindMaximumHandle( _parent );
			world.SetNextHandle( maxHandle + 1);
			ScenePointers::ResolveComponentPointers( world, handleOffset );
			return &newNode;
		}		
	}
}