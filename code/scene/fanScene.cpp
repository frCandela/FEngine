#include "scene/fanScene.hpp"

#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/actors/fanActor.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanPhysicsManager.hpp"
#include "scene/fanComponentPtr.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/fanEcsWorld.hpp"
#include "core/time/fanTime.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanSignal.hpp"
#include "ecs/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/ecs/systems/fanSynchronizeMotionStates.hpp"
#include "scene/ecs/systems/fanRegisterPhysics.hpp"
#include "scene/ecs/systems/fanEmitParticles.hpp"
#include "scene/ecs/systems/fanUpdateParticles.hpp"
#include "scene/ecs/systems/fanGenerateParticles.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Scene::Scene( const std::string _name, void ( *_initializeTypesEcsWorld )( EcsWorld& ) ) :
		m_name( _name )
		, m_path( "" )
		, m_root( nullptr )
		, m_physicsManager( new PhysicsManager( btVector3::Zero() ) )
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
	// Creates a game object and adds it to the scene hierarchy
	//================================================================================================================================
	Gameobject* Scene::CreateGameobject( const std::string /*_name*/, Gameobject * const /*_parent*/, const uint64_t /*_uniqueId*/ )
	{
// 		assert( _uniqueId == 0 || ! FindGameobject( _uniqueId ) );
// 
// 		Gameobject* const parent = _parent == nullptr ? m_root : _parent;
// 		const uint64_t id = _uniqueId == 0 ? m_nextUniqueID++ : _uniqueId;
// 
// 		if ( id >= m_nextUniqueID )
// 		{
// 			m_nextUniqueID = id + 1;
// 		}
// 
// 		Gameobject* gameobject = new Gameobject( _name, parent, this, id );
// 		m_gameobjects[ gameobject->GetUniqueID() ] = gameobject;
// 		return gameobject;
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneNode& Scene::InstanciateSceneNode( const std::string _name, SceneNode* const _parentNode, const bool _generateID )
	{
		SceneNode* const parent = _parentNode == nullptr ? m_rootNode : _parentNode;
		EntityID entityID = m_ecsWorld->CreateEntity();
		EntityHandle handle = m_ecsWorld->CreateHandle( entityID );
		SceneNode& sceneNode = m_ecsWorld->AddComponent<SceneNode>( entityID );

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
			m_ecsWorld->RunSystemOnSubset<S_RegisterAllRigidbodies>( entities, -1.f );
		}

		return prefabRoot;
	}

	//================================================================================================================================
	// Deletes a gameobject and removes it from the scene hierarchy at the end of the frame
	//================================================================================================================================
	void Scene::DeleteGameobject( Gameobject* _gameobject )
	{
		m_entitiesToDelete.push_back( _gameobject );
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
	//================================================================================================================================
	void Scene::R_BuildEntitiesList( Gameobject* _gameobject, std::vector<Gameobject*>& _entitiesList ) const
	{
		_entitiesList.push_back( _gameobject );
		const std::vector<Gameobject*>& childs = _gameobject->GetChilds();
		for ( int childIndex = 0; childIndex < childs.size(); childIndex++ )
		{
			R_BuildEntitiesList( childs[ childIndex ], _entitiesList );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Component* Scene::R_FindComponentOfType( Gameobject* _gameobject, const uint32_t _typeID ) const
	{

		// Search in components
		const std::vector<Component*>& components = _gameobject->GetComponents();
		for ( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
		{
			Component* component = components[ componentIndex ];
			if ( component->GetType() == _typeID )
			{
				return component;
			}
		}

		// Recursive call to child entities
		const std::vector<Gameobject*>& childEntities = _gameobject->GetChilds();
		for ( int childIndex = 0; childIndex < childEntities.size(); childIndex++ )
		{
			Component* component = R_FindComponentOfType( childEntities[ childIndex ], _typeID );
			if ( component != nullptr )
			{
				return component;
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < Gameobject* >  Scene::BuildEntitiesList() const
	{
		std::vector<Gameobject*> entitiesList;
		R_BuildEntitiesList( m_root, entitiesList );
		return entitiesList;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::BeginFrame()
	{
		SCOPED_PROFILE( scene_begin )
			for ( int actorIndex = ( int ) m_startingActors.size() - 1; actorIndex >= 0; --actorIndex )
			{
				Actor* actor = m_startingActors[ actorIndex ];
				if ( actor->GetState() == Actor::STARTING )
				{
					m_activeActors.push_back( actor );
					actor->Start();
					actor->SetState( Actor::ACTIVE );
					m_startingActors.erase( m_startingActors.begin() + actorIndex );
				}
			}
	}

	//================================================================================================================================
	// Deletes every gameobject in the m_toDeleteLater vector
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
			if( m_ecsWorld->HasComponent<Rigidbody2>( entityID ) )
			{
				Rigidbody2& rb = m_ecsWorld->GetComponent<Rigidbody2>( entityID );
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
		BeginFrame();
		{
			//const float delta = m_state == State::PLAYING ? _delta : 0.f;

			//m_ecsManager->UpdatePrePhysics( delta );@hack;
			PhysicsWorld& physicsWorld = m_ecsWorld->GetSingletonComponent<PhysicsWorld>();

			m_ecsWorld->RunSystem<S_SynchronizeMotionStateFromTransform>( _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			m_ecsWorld->RunSystem<S_SynchronizeTransformFromMotionState>( _delta );
			//m_ecsManager->UpdatePostPhysics( delta );@hack

			m_ecsWorld->RunSystem<S_UpdateParticles>( _delta );
			m_ecsWorld->RunSystem<S_EmitParticles>( _delta );
			if(  !m_ecsWorld->RunSystem<S_GenerateParticles>( _delta ) )
			{
				RenderWorld& renderWorld = m_ecsWorld->GetSingletonComponent<RenderWorld>();
				renderWorld.particlesMesh.LoadFromVertices( {} );
			}
			//UpdateActors( _delta );

			//m_ecsManager->Update( delta );@hack
			//LateUpdateActors( _delta );
			//m_ecsManager->LateUpdate( delta );@hack
		}
		EndFrame();

// 		ImGui::Begin( "physicsWorld" );
// 		{
// 			ImGui::PushID( &physicsWorld );
// 			ImGui::Text( "num %d", physicsWorld.dynamicsWorld->getNumCollisionObjects() );
// 			ImGui::PopID();

// 			ImGui::Text( "m_actors         %d", m_actors.size());
// 			ImGui::Text( "m_startingActors %d", m_startingActors.size() );
// 			ImGui::Text( "m_activeActors   %d", m_activeActors.size() );
// 			ImGui::Text( "m_pausedActors   %d", m_pausedActors.size() );
		//}ImGui::End();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::UpdateActors( const float _delta )
	{
		for ( Actor* actor : m_activeActors )
		{
			try
			{
				actor->Update( _delta );
			}
			catch ( ... )
			{
				Debug::Error() << "Update error on actor " << actor->s_name << " of gameobject " << actor->GetGameobject().GetName() << Debug::Endl();
				actor->SetEnabled( false );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::LateUpdateActors( const float _delta )
	{
		SCOPED_PROFILE( scene_L_update )
			for ( Actor* actor : m_activeActors )
			{
				try
				{
					actor->LateUpdate( _delta );
				}
				catch ( ... )
				{
					Debug::Error() << "LateUpdate error on actor " << actor->s_name << " of gameobject " << actor->GetGameobject().GetName() << Debug::Endl();
					actor->SetEnabled( false );
				}

			}
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
	Gameobject* Scene::FindGameobject( const uint64_t _id )
	{
		if ( _id != 0 )
		{
			auto it = m_gameobjects.find( _id );
			if ( it != m_gameobjects.end() )
			{
				return it->second;
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Enable( Actor* _actor )
	{
		switch ( _actor->GetState() )
		{
		case Actor::STOPPED:
			m_startingActors.push_back( _actor );
			_actor->SetState( Actor::STARTING );
			_actor->OnEnable();
			break;
		case Actor::STARTING:
			Debug::Warning( "Trying to enable a started actor" );
			break;
		case Actor::ACTIVE:
			Debug::Warning( "Trying to enable an active actor" );
			break;
		case Actor::PAUSED:
			for ( int actorIndex = ( int ) m_pausedActors.size() - 1; actorIndex >= 0; --actorIndex )
			{
				if ( m_pausedActors[ actorIndex ] == _actor )
				{
					m_pausedActors.erase( m_pausedActors.begin() + actorIndex );
					m_activeActors.push_back( _actor );
					_actor->SetState( Actor::ACTIVE );
					_actor->OnEnable();
				}
			}
			assert( _actor->GetState() == Actor::ACTIVE );
			break;
		default:
			assert( false );
			break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Disable( Actor* _actor )
	{
		if ( _actor->GetGameobject().GetEditorFlags() & Gameobject::EditorFlag::ALWAYS_PLAY_ACTORS ) { return; }

		if ( _actor->GetState() != Actor::STOPPED )
		{
			std::vector< Actor* >& container =
				_actor->GetState() == Actor::STARTING ? m_startingActors
				: _actor->GetState() == Actor::ACTIVE ? m_activeActors
				: m_pausedActors; //    Actor::PAUSED

			for ( int actorIndex = ( int ) container.size() - 1; actorIndex >= 0; --actorIndex )
			{
				if ( container[ actorIndex ] == _actor )
				{
					_actor->OnDisable();

					switch ( _actor->GetState() )
					{
					case Actor::STARTING:
						_actor->SetState( Actor::PAUSED );
						break;
					case Actor::ACTIVE:
						m_activeActors.erase( m_activeActors.begin() + actorIndex );
						m_pausedActors.push_back( _actor );
						_actor->SetState( Actor::PAUSED );
						break;
					case Actor::PAUSED:
						m_pausedActors.erase( m_pausedActors.begin() + actorIndex );
						_actor->SetState( Actor::STOPPED );
						_actor->Stop();
						break;
					default:
						assert( false );
						break;
					}
					return;
				}
			}
			assert( false );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Play()
	{
		if ( m_state == State::STOPPED )
		{
			Debug::Highlight() << m_name << ": play" << Debug::Endl();
			for ( int actorIndex = 0; actorIndex < m_actors.size(); ++actorIndex )
			{
				if ( m_actors[ actorIndex ]->GetState() == Actor::STOPPED )
				{
					Enable( m_actors[ actorIndex ] );
				}
			}
			m_state = State::PLAYING;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Scene::Stop()
	{
		if ( m_state == State::PLAYING || m_state == State::PAUSED )
		{
			for ( int actorIndex = ( int ) m_activeActors.size() - 1; actorIndex >= 0; --actorIndex )
			{
				Disable( m_activeActors[ actorIndex ] );
			}

			for ( int actorIndex = ( int ) m_pausedActors.size() - 1; actorIndex >= 0; --actorIndex )
			{
				Disable( m_pausedActors[ actorIndex ] );
			}

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

			for ( int actorIndex = ( int ) m_activeActors.size() - 1; actorIndex >= 0; --actorIndex )
			{
				Disable( m_activeActors[ actorIndex ] );
			}

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
			for ( int actorIndex = ( int ) m_pausedActors.size() - 1; actorIndex >= 0; --actorIndex )
			{
				Enable( m_pausedActors[ actorIndex ] );
			} m_pausedActors.clear();
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
	void Scene::RegisterActor( Actor* _actor )
	{
		m_actors.push_back( _actor );
		if ( m_state == PLAYING || _actor->GetGameobject().GetEditorFlags() & Gameobject::EditorFlag::ALWAYS_PLAY_ACTORS )
		{
			_actor->SetEnabled( true );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::UnregisterActor( Actor* _actor )
	{
		std::vector< Actor* >::iterator  itGlobal = std::find( m_actors.begin(), m_actors.end(), _actor );
		std::vector< Actor* >::iterator  itStarting = std::find( m_startingActors.begin(), m_startingActors.end(), _actor );
		std::vector< Actor* >::iterator  itActive = std::find( m_activeActors.begin(), m_activeActors.end(), _actor );

		if ( itGlobal != m_actors.end() ) { *itGlobal = *( m_actors.end() - 1 ); m_actors.pop_back(); }
		if ( itStarting != m_startingActors.end() ) { *itStarting = *( m_startingActors.end() - 1 ); m_startingActors.pop_back(); }
		if ( itActive != m_activeActors.end() ) { *itActive = *( m_activeActors.end() - 1 ); m_activeActors.pop_back(); }

	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Clear()
	{
		m_ecsWorld->RunSystem<S_UnregisterAllRigidbodies>( -1.f );
		m_path = "";
		m_instantiate->Clear();
		DeleteNodesImmediate( { m_rootNode } );
		m_rootNode = nullptr;
		m_startingActors.clear();
		m_activeActors.clear();
		m_pausedActors.clear();
		m_entitiesToDelete.clear();
		m_sceneNodesToDelete.clear();
		m_gameobjects.clear();
		m_root = nullptr;
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
				ecComponent& component = world.GetComponentAt( entityID, componentIndex );
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

			m_ecsWorld->RunSystem<S_RegisterAllRigidbodies>( -1.f );
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
				ecComponent& component			    = world.AddComponent( entityID, componentIndex );				
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

	//================================================================================================================================
	//================================================================================================================================
	void Scene::RegisterDirectionalLight( DirectionalLight* _directionalLight )
	{
		// Looks for the _directionalLight
		for ( int lightIndex = 0; lightIndex < m_directionalLights.size(); lightIndex++ )
		{
			if ( m_directionalLights[ lightIndex ] == _directionalLight )
			{
				Debug::Get() << Debug::Severity::warning << "Directional Light already registered in gameobject : " << _directionalLight->GetGameobject().GetName() << Debug::Endl();
				return;
			}
		}

		// Check num lights
		if ( m_directionalLights.size() >= RenderGlobal::s_maximumNumDirectionalLight )
		{
			Debug::Get() << Debug::Severity::warning << "Too much lights in the scene, maximum is " << RenderGlobal::s_maximumNumDirectionalLight << Debug::Endl();
		}
		else
		{
			m_directionalLights.push_back( _directionalLight );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::UnRegisterDirectionalLight( DirectionalLight* _directionalLight )
	{

		const size_t num = m_directionalLights.size();

		// Removes the light
		for ( int lightIndex = 0; lightIndex < m_directionalLights.size(); lightIndex++ )
		{
			if ( m_directionalLights[ lightIndex ] == _directionalLight )
			{
				m_directionalLights.erase( m_directionalLights.begin() + lightIndex );
			}
		}

		// Light not removed
		if ( m_directionalLights.size() == num )
		{
			Debug::Get() << Debug::Severity::warning << "Trying to remove a non registered directional light! gameobject=" << _directionalLight->GetGameobject().GetName() << Debug::Endl();
			return;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::RegisterPointLight( PointLight* _pointLight )
	{
		// Looks for the _pointLight
		for ( int lightIndex = 0; lightIndex < m_pointLights.size(); lightIndex++ )
		{
			if ( m_pointLights[ lightIndex ] == _pointLight )
			{
				Debug::Get() << Debug::Severity::warning << "PointLight already registered in gameobject : " << _pointLight->GetGameobject().GetName() << Debug::Endl();
				return;
			}
		}

		// Check num lights
		if ( m_pointLights.size() >= RenderGlobal::s_maximumNumPointLights )
		{
			Debug::Get() << Debug::Severity::warning << "Too much lights in the scene, maximum is " << RenderGlobal::s_maximumNumPointLights << Debug::Endl();
		}
		else
		{
			m_pointLights.push_back( _pointLight );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::UnRegisterPointLight( PointLight* _pointLight )
	{
		const size_t num = m_pointLights.size();

		// Removes the light
		for ( int lightIndex = 0; lightIndex < m_pointLights.size(); lightIndex++ )
		{
			if ( m_pointLights[ lightIndex ] == _pointLight )
			{
				m_pointLights.erase( m_pointLights.begin() + lightIndex );
			}
		}

		// Light not removed
		if ( m_pointLights.size() == num )
		{
			Debug::Get() << Debug::Severity::warning << "Trying to remove a non registered point light! gameobject=" << _pointLight->GetGameobject().GetName() << Debug::Endl();
			return;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::RegisterMeshRenderer( MeshRenderer* _meshRenderer )
	{
		// Looks for the model
		for ( int modelIndex = 0; modelIndex < m_meshRenderers.size(); modelIndex++ )
		{
			if ( m_meshRenderers[ modelIndex ] == _meshRenderer )
			{
				Debug::Get() << Debug::Severity::warning << "MeshRenderer already registered : " << _meshRenderer->GetGameobject().GetName() << Debug::Endl();
				return;
			}
		}
		m_meshRenderers.push_back( _meshRenderer );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::UnRegisterMeshRenderer( MeshRenderer* _meshRenderer )
	{
		for ( int modelIndex = 0; modelIndex < m_meshRenderers.size(); modelIndex++ )
		{
			if ( m_meshRenderers[ modelIndex ] == _meshRenderer )
			{
				m_meshRenderers.erase( m_meshRenderers.begin() + modelIndex );
			}
		}
	}
}