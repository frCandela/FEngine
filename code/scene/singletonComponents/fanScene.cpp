#include "scene/singletonComponents/fanScene.hpp"

#include "core/time/fanTime.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanSignal.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanComponentPtr.hpp"
#include "scene/fanSceneTags.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( Scene );

	//================================================================================================================================
	//================================================================================================================================
	void Scene::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::SCENE16;
		_info.init = &Scene::Init; 
		_info.onGui = &Scene::OnGui;
		_info.name = "scene";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Init( SingletonComponent& _component )
	{
		Scene& scene = static_cast<Scene&>( _component );
		scene.path = "";
		scene.root = nullptr;
		scene.nextUniqueID = 1;
		scene.mainCamera = nullptr;
		scene.sceneNodesToDelete.clear();
		scene.world = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::OnGui( SingletonComponent& _component )
	{
		Scene& scene = static_cast<Scene&>( _component );
		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "path: %s", scene.path.c_str() );
			ImGui::Text( "nextUniqueID: %d", scene.nextUniqueID );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================
	Scene::~Scene()
	{
		Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneNode& Scene::CreateSceneNode( const std::string _name, SceneNode* const _parentNode, const bool _generateID )
	{		
		EntityID entityID = world->CreateEntity();
		EntityHandle handle = world->CreateHandle( entityID );
		
		SceneNode* const parent = _parentNode == nullptr ? root : _parentNode;
		SceneNode& sceneNode = world->AddComponent<SceneNode>( entityID );
		world->AddComponent<Bounds>( entityID );
		world->AddTag<tag_boundsOutdated>( entityID );

		uint32_t id = _generateID ? nextUniqueID++ : 0;
		sceneNode.Build( _name, *this, handle, id, parent );

		return sceneNode;
	}

	//================================================================================================================================
	// Creates a game object from a prefab and adds it to the scene hierarchy
	// Gameobjects ids are remapped depending on the scene next id
	//================================================================================================================================
	SceneNode* Scene::CreatePrefab( const Prefab& _prefab, SceneNode * const _parent )
	{
		// instantiate prefab
		SceneNode* const parent = _parent == nullptr ? root : _parent;
		SceneNode * prefabRoot = _prefab.Instanciate( *parent );

		// registers newly added rigidbodies
		if( prefabRoot != nullptr )
		{
			std::vector<SceneNode*> nodes;
			SceneNode::GetDescendantsOf( *prefabRoot, nodes );

			for ( SceneNode* node : nodes )
			{
				EntityID entityID = world->GetEntityID( node->handle );
				if( world->HasComponent<Rigidbody>( entityID ) )
				{
					onCreateRigidbody.Emmit( entityID );
				}
			}
		}
		return prefabRoot;
	}

	//================================================================================================================================
	//================================================================================================================================
	uint32_t Scene::R_FindMaximumId( SceneNode& _node )
	{
		uint32_t id = _node.uniqueID;
		const std::vector<SceneNode*>& childs = _node.childs;
		for ( int childIndex = 0; childIndex < childs.size(); childIndex++ )
		{
			uint32_t childId = R_FindMaximumId( *childs[ childIndex ] );
			if ( childId > id )
			{
				id = childId;
			}
		}
		return id;
	}

	//================================================================================================================================
	// clears a set of scene nodes
	// warning : very expensive & invalidates all EntityID
	//================================================================================================================================
	void Scene::DeleteNodesImmediate( const std::vector<SceneNode*>& _nodes )
	{		
		// stacks of initial nodes
		std::stack<SceneNode* > nodesstack;
		for( int nodeIndex = 0; nodeIndex < _nodes.size(); nodeIndex++ )
		{
			if( _nodes[nodeIndex] != nullptr )
			{
				nodesstack.push( _nodes[nodeIndex] );
			}			
		}

		// find all child nodes
		std::set<SceneNode* > nodesToDelete;
		while( !nodesstack.empty() )
		{
			SceneNode& node = *nodesstack.top();
			nodesstack.pop();
			nodesToDelete.insert( &node );
			for( int childIndex = 0; childIndex < node.childs.size(); childIndex++ )
			{
				nodesstack.push( node.childs[childIndex] );
			}
		}

		// delete all nodes
		for( SceneNode* node : nodesToDelete )
		{
			EntityID entityID = world->GetEntityID( node->handle );

			// remove rigidbody from physics world
			if( world->HasComponent<Rigidbody>( entityID ) )
			{
				onDeleteRigidbody.Emmit( entityID );
			}
			world->KillEntity( entityID );
			if( node->parent != nullptr )
			{
				node->parent->RemoveChild( *node );
			}
		}
		world->SortEntities();
		world->RemoveDeadEntities();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::SetMainCamera( SceneNode& _nodeCamera )
	{
		if( &_nodeCamera != mainCamera )
		{
			mainCamera = &_nodeCamera;
			onSetMainCamera.Emmit( _nodeCamera );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Clear()
	{
		onClear.Emmit(*this);
		path = "";
		//instantiate->Clear();
		DeleteNodesImmediate({ root } );
		root = nullptr;
		sceneNodesToDelete.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::New()
	{
		Clear( );
		nextUniqueID = 1;
		root = &CreateSceneNode( "root", nullptr );
		onLoad.Emmit( *this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Save( ) const
	{
		std::ofstream outStream( path );
		if ( outStream.is_open() )
		{
			Json json;

			// scene global parameters
			Json& jScene = json["scene"];
			{
				Serializable::SaveString( jScene, "path", path );
			}

			// save singleton components
			Json& jSingletons = jScene["singletons"];
			unsigned nextIndex = 0;
			const std::vector< SingletonComponentInfo >& singletonsInfos = world->GetVectorSingletonComponentInfo();
			for ( const SingletonComponentInfo& info : singletonsInfos )
			{
				if( info.save != nullptr )
				{
					Json& jSingleton_i = jSingletons[nextIndex++];
					Serializable::SaveUInt( jSingleton_i, "singleton_id", info.staticIndex );
					Serializable::SaveString( jSingleton_i, "singleton", info.name );
					SingletonComponent& singleton = world->GetSingletonComponent( info.staticIndex );
					info.save( singleton, jSingleton_i );
				}
			}

			// saves all scene nodes recursively
			Json& jRoot = jScene["root"];
			R_SaveToJson( *root, jRoot );
			RemapSceneNodesIndices( json );				
			outStream << json; // write to disk			
			outStream.close();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::R_SaveToJson( const SceneNode& _node, Json& _json )
	{	
		EcsWorld& world = *_node.scene->world;
		Serializable::SaveString( _json, "name", _node.name );
		Serializable::SaveUInt( _json, "node_id", _node.uniqueID );

		// save components
		Json& jComponents = _json["components"];
		{
			EntityID entityID = world.GetEntityID( _node.handle );
			unsigned nextIndex = 0;
			for( int componentIndex = 0; componentIndex < world.GetComponentCount(entityID) ; componentIndex++ )
			{
				// if a save method is provided, saves the component
				Component& component = world.GetComponentAt( entityID, componentIndex );
				const ComponentInfo& info = world.GetComponentInfo( component.GetIndex() );								
				if( info.save != nullptr )
				{
					Json& jComponent_i = jComponents[nextIndex++];
					Serializable::SaveUInt( jComponent_i, "component_id", info.staticIndex );
					Serializable::SaveString( jComponent_i, "type", info.name );
					info.save( component, jComponent_i );
				}				
			}
		}

		// save childs
		Json& jchilds = _json["childs"];
		unsigned childIndex = 0;
		for( int sceneNodeIndex = 0; sceneNodeIndex < _node.childs.size(); sceneNodeIndex++ )
		{
			SceneNode& childNode = *_node.childs[sceneNodeIndex];
			if( ! childNode.HasFlag( SceneNode::NOT_SAVED ) )
			{
				Json& jchild = jchilds[childIndex];
				R_SaveToJson( childNode, jchild );
				++childIndex;
			}
		}		
	}

	//================================================================================================================================
	// Find all the gameobjects indices in the json and remap them on a range close to zero
	// ex: 400, 401, 1051 will be remapped to 1,2,3
	//================================================================================================================================
	void Scene::RemapSceneNodesIndices( Json& _json )
	{
		std::vector< Json* > jsonIndices;
		std::set< uint64_t > uniqueIndices;

		// parse all the json and get all the gameobjects ids
		std::stack< Json* > stack;
		stack.push( &_json );
		while( !stack.empty() )
		{
			Json& js = *stack.top();
			stack.pop();

			Json::iterator& gameobjectId = js.find( "node_id" );
			if( gameobjectId != js.end() )
			{
				uint64_t value = *gameobjectId;
				if( value != 0 )
				{
					jsonIndices.push_back( &( *gameobjectId ) );
					uniqueIndices.insert( value );
				}
			}

			if( js.is_structured() )
			{
				for( auto& element : js )
				{
					stack.push( &element );
				}
			}
		}

		// creates the remap table
		std::map< uint64_t, uint64_t > remapTable;
		int remap = 1;
		for( uint64_t uniqueIndex : uniqueIndices )
		{
			remapTable[uniqueIndex] = remap++;
		}

		// remap all indices
		for( int jsonIdIndex = 0; jsonIdIndex < jsonIndices.size(); jsonIdIndex++ )
		{
			Json& js = *jsonIndices[jsonIdIndex];
			const uint64_t value = js;
			js = remapTable[value];
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::LoadFrom( const std::string _path )
	{
		Clear();
 		std::ifstream inStream( _path );
 		if ( inStream.is_open() && inStream.good() )
 		{
 			// Load scene
			Json sceneJson;
			inStream >> sceneJson;

			// scene global parameters
			const Json& jScene = sceneJson["scene"];
			{
				Serializable::LoadString( jScene, "path", path );
			}

			// load singleton components
			if( jScene.find( "singletons" ) != jScene.end() )
			{
				const Json& jSingletons = jScene["singletons"];
				for( int childIndex = 0; childIndex < jSingletons.size(); childIndex++ )
				{
					const Json& jSingleton_i = jSingletons[childIndex];
					unsigned staticIndex = 0;
					Serializable::LoadUInt( jSingleton_i, "singleton_id", staticIndex );
					const SingletonComponentInfo& info = world->GetSingletonComponentInfo( staticIndex );
					SingletonComponent& singleton = world->GetSingletonComponent( staticIndex );
					info.load( singleton, jSingleton_i );
				}
			}			

			// loads all nodes recursively
			const Json& jRoot = jScene["root"];
			root = &CreateSceneNode( "root", nullptr );
			R_LoadFromJson( jRoot, *root, 0 );
			
			path = _path;
			inStream.close();
			nextUniqueID = R_FindMaximumId( *root ) + 1;

			//m_instantiate->ResolveGameobjectPtr( 0 );
			//m_instantiate->ResolveComponentPtr( 0 );

			// @hack : sets the sunlight mesh on loading
			Signature signature = world->GetSignature<MeshRenderer>() | world->GetSignature<SceneNode>();
			std::vector<EntityID> entities = world->Match( signature );
			for ( EntityID entityID : entities )
			{
				SceneNode& node = world->GetComponent<SceneNode>( entityID );
				if( node.name == "sun_light" )
				{
					MeshRenderer& meshRenderer = world->GetComponent<MeshRenderer>( entityID );
					SunLight& sunlight = world->GetSingletonComponent<SunLight>();
					meshRenderer.mesh = &sunlight.mesh;
				}
			}

			onLoad.Emmit( *this );
			return true;
		}
		else
		{
			Debug::Get() << Debug::Severity::error << "failed to open file " << _path << Debug::Endl();
			New();
			return false;
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::R_LoadFromJson( const Json& _json, SceneNode& _node, const uint32_t _idOffset )
	{
		//ScopedTimer timer("load scene");
		EcsWorld& world = *_node.scene->world;

		Serializable::LoadString( _json, "name", _node.name );
		Serializable::LoadUInt( _json, "node_id", _node.uniqueID );
		_node.uniqueID += _idOffset;

		// components
		const Json& jComponents = _json["components"];
		{
			const EntityID		 entityID = world.GetEntityID( _node.handle );
			for( int childIndex = 0; childIndex < jComponents.size(); childIndex++ )
			{
				const Json& jComponent_i = jComponents[childIndex];				
				unsigned staticIndex = 0;
				Serializable::LoadUInt( jComponent_i, "component_id", staticIndex );
				const ComponentIndex componentIndex = world.GetDynamicIndex(staticIndex);
				const ComponentInfo& info			= world.GetComponentInfo( componentIndex );				
				Component& component			    = world.AddComponent( entityID, componentIndex );				
				info.load( component, jComponent_i );
			}
		}

		// Load childs
		const Json& jchilds = _json["childs"];
		{
			for( int childIndex = 0; childIndex < jchilds.size(); childIndex++ )
			{
				const Json& jchild_i = jchilds[childIndex];
				{
					SceneNode& childNode = _node.scene->CreateSceneNode( "tmp", &_node, false );
					R_LoadFromJson( jchild_i, childNode, _idOffset );
				}
			}
		}
	}
}