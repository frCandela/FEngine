#include "scene/fanScene.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanComponentPtr.hpp"
#include "core/time/fanTime.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanSignal.hpp"
#include "ecs/components/fanBounds.hpp"
#include "ecs/components/fanRigidbody.hpp"
#include "scene/ecs/systems/fanSynchronizeMotionStates.hpp"
#include "scene/ecs/systems/fanRegisterPhysics.hpp"
#include "scene/ecs/systems/fanUpdateParticles.hpp"
#include "scene/ecs/systems/fanEmitParticles.hpp"
#include "scene/ecs/systems/fanGenerateParticles.hpp"
#include "scene/ecs/systems/fanUpdateBounds.hpp"
#include "ecs/singletonComponents/fanPhysicsWorld.hpp"
#include "ecs/singletonComponents/fanRenderWorld.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Scene::Scene( const std::string _name, void ( *_initializeTypesEcsWorld )( EcsWorld& ) ) :
		m_name( _name )
		, m_path( "" )
		, m_ecsWorld( new EcsWorld( _initializeTypesEcsWorld ) )
		, m_instantiate( new SceneInstantiate( *this ) )
	{}

	//================================================================================================================================
	//================================================================================================================================
	Scene::~Scene()
	{
		Clear();

		//delete m_physicsManager; @hack
		delete m_ecsWorld;
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneNode& Scene::InstanciateSceneNode( const std::string _name, SceneNode* const _parentNode, const bool _generateID )
	{
		SceneNode* const parent = _parentNode == nullptr ? m_rootNode : _parentNode;
		EntityID entityID = m_ecsWorld->CreateEntity();
		EntityHandle handle = m_ecsWorld->CreateHandle( entityID );
		SceneNode& sceneNode = m_ecsWorld->AddComponent<SceneNode>( entityID );
		m_ecsWorld->AddComponent<Bounds>( entityID );
		m_ecsWorld->AddTag<tag_boundsOutdated>( entityID );

		uint32_t id = _generateID ? nextUniqueID++ : 0;
		sceneNode.Build( _name, *this, handle, id, parent );

		return sceneNode;
	}

	//================================================================================================================================
	// deletes the scene node at the end of the frame
	//================================================================================================================================
	void Scene::DeleteSceneNode( SceneNode& _node )
	{
		m_sceneNodesToDelete.push_back( &_node );
	}

	//================================================================================================================================
	// Creates a game object from a prefab and adds it to the scene hierarchy
	// Gameobjects ids are remapped depending on the scene next id
	//================================================================================================================================
	SceneNode* Scene::InstanciatePrefab( const Prefab& _prefab, SceneNode * const _parent )
	{
		// instanciate prefab
		SceneNode* const parent = _parent == nullptr ? m_rootNode : _parent;
		SceneNode * prefabRoot = _prefab.Instanciate( *parent );

		// registers newly added rigidbodies
		if( prefabRoot != nullptr )
		{
			std::vector<SceneNode*> nodes;
			SceneNode::GetDescendantsOf( *prefabRoot, nodes );

			std::vector<EntityID> entities;
			entities.reserve( nodes.size() );
			for ( SceneNode* node : nodes )
			{
				entities.push_back( m_ecsWorld->GetEntityID( node->entityHandle ) );
			}
			Signature signature = S_RegisterAllRigidbodies::GetSignature( *m_ecsWorld );
			S_RegisterAllRigidbodies::Run( *m_ecsWorld,  m_ecsWorld->MatchSubset( signature, entities ) );
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
	// Deletes every scene node in the m_sceneNodesToDelete vector
	//================================================================================================================================
	void Scene::EndFrame()
	{
		SCOPED_PROFILE( scene_endFrame );

		// deletes scene nodes
		DeleteNodesImmediate( m_sceneNodesToDelete );
		m_sceneNodesToDelete.clear();

		m_ecsWorld->SortEntities();
		m_ecsWorld->RemoveDeadEntities();
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
		PhysicsWorld& physicsWorld = m_ecsWorld->GetSingletonComponent<PhysicsWorld>();
		for( SceneNode* node : nodesToDelete )
		{
			EntityID entityID = m_ecsWorld->GetEntityID( node->entityHandle );

			// remove rigidbody from physics world
			if( m_ecsWorld->HasComponent<Rigidbody>( entityID ) )
			{
				Rigidbody& rb = m_ecsWorld->GetComponent<Rigidbody>( entityID );
				physicsWorld.dynamicsWorld->removeRigidBody( &rb.rigidbody );
			}
			m_ecsWorld->KillEntity( entityID );
			if( node->parent != nullptr )
			{
				node->parent->RemoveChild( *node );
			}
		}		

		m_ecsWorld->SortEntities();
		m_ecsWorld->RemoveDeadEntities();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Update( const float _delta )
	{
		SCOPED_PROFILE( scene_update );
		{
			//const float delta = m_state == State::PLAYING ? _delta : 0.f;

			//RUN_SYSTEM( ecsPlanetsSystem, Run );
			
			const Signature signatureSMSFT = S_SynchronizeMotionStateFromTransform::GetSignature( *m_ecsWorld );
			const Signature signatureSTFMS = S_SynchronizeTransformFromMotionState::GetSignature( *m_ecsWorld );
			const Signature signatureUpdateParticles = S_UpdateParticles::GetSignature( *m_ecsWorld );
			const Signature signatureEmitParticles = S_EmitParticles::GetSignature( *m_ecsWorld );
			const Signature signatureGenParticles = S_GenerateParticles::GetSignature( *m_ecsWorld );
			const Signature signatureUpdateBoundsFromRigidbody = S_UpdateBoundsFromRigidbody::GetSignature( *m_ecsWorld );
			const Signature signatureUpdateBoundsFromModel = S_UpdateBoundsFromModel::GetSignature( *m_ecsWorld );
			const Signature signatureUpdateBoundsFromTransform = S_UpdateBoundsFromTransform::GetSignature( *m_ecsWorld );

			// physics
			PhysicsWorld& physicsWorld = m_ecsWorld->GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( *m_ecsWorld, m_ecsWorld->Match( signatureSMSFT ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( *m_ecsWorld, m_ecsWorld->Match( signatureSTFMS ), _delta );

			// particles
			S_UpdateParticles::Run( *m_ecsWorld, m_ecsWorld->Match( signatureUpdateParticles ), _delta );
			S_EmitParticles::Run( *m_ecsWorld, m_ecsWorld->Match( signatureEmitParticles ), _delta );
			//RUN_SYSTEM( ecsParticleSunlightOcclusionSystem, Run );

			// clears particles mesh
			S_GenerateParticles::Run( *m_ecsWorld, m_ecsWorld->Match( signatureGenParticles ), _delta );

			//UpdateActors( _delta );

			
			//RUN_SYSTEM( ecsSolarEruptionMeshSystem, Run );

			//LateUpdateActors( _delta );
			S_UpdateBoundsFromRigidbody::Run( *m_ecsWorld, m_ecsWorld->Match( signatureUpdateBoundsFromRigidbody ), _delta );
			S_UpdateBoundsFromModel::Run( *m_ecsWorld, m_ecsWorld->Match( signatureUpdateBoundsFromModel ), _delta );
			S_UpdateBoundsFromTransform::Run( *m_ecsWorld, m_ecsWorld->Match( signatureUpdateBoundsFromTransform ), _delta );
			
			//RUN_SYSTEM( ecsUpdateBullet, Run );
		}
		EndFrame();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::SetMainCamera( SceneNode& _nodeCamera )
	{
		if ( &_nodeCamera != m_mainCamera )
		{
			m_mainCamera = &_nodeCamera;
			onSetMainCamera.Emmit( _nodeCamera );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Play()
	{
		if ( m_state == State::STOPPED )
		{
			Debug::Highlight() << m_name << ": play" << Debug::Endl();
			m_state = State::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Scene::Stop()
	{
		if ( m_state == State::PLAYING || m_state == State::PAUSED )
		{

			Debug::Highlight() << m_name << ": stopped" << Debug::Endl();
			m_state = State::STOPPED;
			onSceneStop.Emmit( *this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Pause()
	{
		if ( m_state == State::PLAYING )
		{
			Debug::Highlight() << m_name << ": paused" << Debug::Endl();
			m_state = State::PAUSED;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Resume()
	{
		if ( m_state == State::PAUSED )
		{
			Debug::Highlight() << m_name << ": resumed" << Debug::Endl();
			m_state = State::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Scene::Step( const float _delta )
	{
		if ( m_state == State::PAUSED )
		{
			Resume();
			Update( _delta );
			Pause();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Clear()
	{
		const Signature signatureUnregisterAllRigidbodies = S_UnregisterAllRigidbodies::GetSignature( *m_ecsWorld );
		S_UnregisterAllRigidbodies::Run( *m_ecsWorld, m_ecsWorld->Match( signatureUnregisterAllRigidbodies ) );

		m_path = "";
		m_instantiate->Clear();
		DeleteNodesImmediate( { m_rootNode } );
		m_rootNode = nullptr;
		m_sceneNodesToDelete.clear();
		onSceneClear.Emmit();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::New()
	{
		Stop();
		Clear();
		nextUniqueID = 1;
		m_rootNode = & InstanciateSceneNode( "root", nullptr );
		onSceneLoad.Emmit( *this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Save() const
	{
		Debug::Get() << Debug::Severity::log << "saving scene: " << m_name << Debug::Endl();
		std::ofstream outStream( m_path );
		if ( outStream.is_open() )
		{
			Json json;

			// scene global parameters
			Json& jScene = json["scene"];
			{
				Serializable::SaveString( jScene, "name", m_name );
				Serializable::SaveString( jScene, "path", m_path );
			}

			// saves all nodes recursively
			Json& jRoot = jScene["root"];
			R_SaveToJson( *m_rootNode, jRoot );
			RemapSceneNodesIndices( json );				
			outStream << json; // write to disk			
			outStream.close();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::R_SaveToJson( const SceneNode& _node, Json& _json )
	{	
		EcsWorld& world = _node.scene->GetWorld();

		Serializable::SaveString( _json, "name", _node.name );
		Serializable::SaveUInt( _json, "node_id", _node.uniqueID );

		// save components
		Json& jComponents = _json["components"];
		{
			EntityID entityID = world.GetEntityID( _node.entityHandle );
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
	// Find all the gameobject indices in the json and remap them on a range close to zero
	// ex: 400, 401, 1051 will be remapped to 1,2,3
	//================================================================================================================================
	void Scene::RemapSceneNodesIndices( Json& _json )
	{
		std::vector< Json* > jsonIndices;
		std::set< uint64_t > uniqueIndices;

		// parse all the json and get all the gameobject ids
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
		Stop();
		Clear();
 		std::ifstream inStream( _path );
 		if ( inStream.is_open() && inStream.good() )
 		{
 			// Load scene
 			Debug::Get() << Debug::Severity::log << "loading scene: " << _path << Debug::Endl();

			Json sceneJson;
			inStream >> sceneJson;

			// scene global parameters
			const Json& jScene = sceneJson["scene"];
			{
				Serializable::LoadString( jScene, "name", m_name );
				Serializable::LoadString( jScene, "path", m_path );
			}

			// loads all nodes recursively
			const Json& jRoot = jScene["root"];
			m_rootNode = &InstanciateSceneNode( "root", nullptr );
			R_LoadFromJson( jRoot, *m_rootNode, 0 );
			
			m_path = _path;
			inStream.close();
			nextUniqueID = R_FindMaximumId( *m_rootNode ) + 1;

			const Signature signatureRegisterAllRigidbodies = S_RegisterAllRigidbodies::GetSignature( *m_ecsWorld );
			S_RegisterAllRigidbodies::Run( *m_ecsWorld, m_ecsWorld->Match( signatureRegisterAllRigidbodies ) );
			//m_instantiate->ResolveGameobjectPtr( 0 );
			//m_instantiate->ResolveComponentPtr( 0 );

			onSceneLoad.Emmit( *this );
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
		EcsWorld& world = _node.scene->GetWorld();


		Serializable::LoadString( _json, "name", _node.name );
		Serializable::LoadUInt( _json, "node_id", _node.uniqueID );
		_node.uniqueID += _idOffset;

		// components
		const Json& jComponents = _json["components"];
		{
			const EntityID		 entityID = world.GetEntityID( _node.entityHandle );
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
					SceneNode& childNode = _node.scene->InstanciateSceneNode( "tmp", &_node, false );
					R_LoadFromJson( jchild_i, childNode, _idOffset );
				}
			}
		}
	}
}