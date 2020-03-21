#include "scene/fanPrefab.hpp"

#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "ecs/fanEcsWorld.hpp"

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

		Scene& scene = *_node.scene;
		Json& prefabJson = m_json[ "prefab" ];
		Scene::R_SaveToJson( _node, prefabJson );
		Scene::RemapSceneNodesIndices( m_json );
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
			const uint32_t idOffset = _parent.scene->nextUniqueID - 1;
			SceneNode& node = _parent.scene->CreateSceneNode( "tmp", nullptr, false );
			EcsWorld& world = *_parent.scene->world;
			EntityID indexStart = (EntityID)world.GetNumEntities();
			Scene::R_LoadFromJson( m_json["prefab"], node, idOffset );
			_parent.scene->nextUniqueID = Scene::R_FindMaximumId( _parent ) + 1;

// 			ResolveGameobjectPtr( idOffset );
// 			ResolveComponentPtr( idOffset );
// 			m_unresolvedGameobjectPtr.clear();
// 			m_unresolvedComponentPtr.clear();

			return &node;
		}
	}
}