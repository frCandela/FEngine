#include "scene/fanScene.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/actors/fanActor.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanComponentPtr.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanPhysicsManager.hpp"
#include "core/fanSignal.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Scene::Scene( const std::string _name ) :
		m_name( _name )
		, m_path( "" )
		, m_root( nullptr )
		, m_ecsManager( new EcsManager() )
		, m_physicsManager( new PhysicsManager( btVector3::Zero() ) )
		, m_instantiate( new SceneInstantiate( *this ) )
	{}

	//================================================================================================================================
	//================================================================================================================================
	Scene::~Scene()
	{
		Clear();

		//delete m_physicsManager; @hack
		delete m_ecsManager;
	}

	//================================================================================================================================
	// Creates a game object and adds it to the scene hierarchy
	//================================================================================================================================
	Gameobject* Scene::CreateGameobject( const std::string _name, Gameobject * const _parent, const uint64_t _uniqueId )
	{
		assert( _uniqueId == 0 || ! FindGameobject( _uniqueId ) );

		Gameobject* const parent = _parent == nullptr ? m_root : _parent;
		const uint64_t id = _uniqueId == 0 ? NextUniqueID() : _uniqueId;

		if ( id >= m_nextUniqueID )
		{
			m_nextUniqueID = id + 1;
		}

		Gameobject* gameobject = new Gameobject( _name, parent, this, id );
		m_gameobjects[ gameobject->GetUniqueID() ] = gameobject;
		return gameobject;
	}

	//================================================================================================================================
	// Creates a game object from a prefab and adds it to the scene hierarchy
	// Gameobjects ids are remapped depending on the scene next id
	//================================================================================================================================
	Gameobject* Scene::CreateGameobject( const Prefab& _prefab, Gameobject * const _parent )
	{
		Gameobject* const parent = _parent == nullptr ? m_root : _parent;
		return m_instantiate->InstanciatePrefab( _prefab, *parent );
	}

	//================================================================================================================================
	// Deletes a gameobject and removes it from the scene hierarchy at the end of the frame
	//================================================================================================================================
	void Scene::DeleteGameobject( Gameobject* _gameobject )
	{
		if ( _gameobject->IsReferenced() )
		{
			m_instantiate->UnregisterPointersForGameobject( _gameobject );
		}
		m_entitiesToDelete.push_back( _gameobject );
	}


	//================================================================================================================================
	//================================================================================================================================
	uint64_t Scene::R_FindMaximumId( Gameobject& _gameobject )
	{
		uint64_t id = _gameobject.GetUniqueID();
		const std::vector<Gameobject*>& childs = _gameobject.GetChilds();
		for ( int childIndex = 0; childIndex < childs.size(); childIndex++ )
		{
			uint64_t childId = R_FindMaximumId( *childs[ childIndex ] );
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
		SCOPED_PROFILE( scene_endFrame )

			// Delete components 
			for ( int componentToDeleteIndex = 0; componentToDeleteIndex < m_componentsToDelete.size(); componentToDeleteIndex++ )
			{
				m_componentsToDelete[ componentToDeleteIndex ]->GetGameobject().RemoveComponent( m_componentsToDelete[ componentToDeleteIndex ] );
			}
		m_componentsToDelete.clear();

		// Delete entities 
		std::set<Gameobject*> deletedEntitiesSet;
		for ( int gameobjectToDeleteIndex = 0; gameobjectToDeleteIndex < m_entitiesToDelete.size(); gameobjectToDeleteIndex++ )
		{
			Gameobject* gameobjectDelete = m_entitiesToDelete[ gameobjectToDeleteIndex ];
			R_DeleteGameobject( gameobjectDelete, deletedEntitiesSet );
		}
		m_entitiesToDelete.clear();

		m_ecsManager->Refresh();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Update( const float _delta )
	{
		SCOPED_PROFILE( scene_update )
			BeginFrame();

		const float delta = m_state == State::PLAYING ? _delta : 0.f;

		m_ecsManager->GetSingletonComponents().GetComponent<ecsCameraPosition_s>().position = m_mainCamera->GetGameobject().GetTransform().GetPosition();


		m_ecsManager->UpdatePrePhysics( delta );
		m_physicsManager->StepSimulation( delta );
		m_ecsManager->UpdatePostPhysics( delta );
		UpdateActors( _delta );
		m_ecsManager->Update( delta );
		LateUpdateActors( _delta );
		m_ecsManager->LateUpdate( delta );
		EndFrame();

		// 		ImGui::Begin( "testoss" );
		// 		{
		// 			ImGui::Text( "m_actors         %d", m_actors.size());
		// 			ImGui::Text( "m_startingActors %d", m_startingActors.size() );
		// 			ImGui::Text( "m_activeActors   %d", m_activeActors.size() );
		// 			ImGui::Text( "m_pausedActors   %d", m_pausedActors.size() );
		// 		}ImGui::End();

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
	void Scene::SetMainCamera( Camera* _camera )
	{
		if ( _camera != m_mainCamera )
		{
			m_mainCamera = _camera;
			onSetMainCamera.Emmit( _camera );
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
			onSceneStop.Emmit( this );
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
	void  Scene::R_DeleteGameobject( Gameobject* _gameobject, std::set<Gameobject*>& _deletedEntitiesSet )
	{
		if ( _gameobject != nullptr && _deletedEntitiesSet.find( _gameobject ) == _deletedEntitiesSet.end() )
		{

			const std::vector<Gameobject*> childs = _gameobject->GetChilds(); // copy
			for ( int childIndex = 0; childIndex < childs.size(); childIndex++ )
			{
				R_DeleteGameobject( childs[ childIndex ], _deletedEntitiesSet );
			}

			onDeleteGameobject.Emmit( _gameobject );
			_deletedEntitiesSet.insert( _gameobject );
			if ( _gameobject->GetParent() != nullptr )
			{
				_gameobject->GetParent()->RemoveChild( _gameobject );
			}
			m_gameobjects.erase( _gameobject->GetUniqueID() );
			delete( _gameobject );
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
		m_path = "";
		m_instantiate->Clear();
		std::set<Gameobject*> deletedEntitiesSet;
		R_DeleteGameobject( m_root, deletedEntitiesSet );
		m_startingActors.clear();
		m_activeActors.clear();
		m_pausedActors.clear();
		m_entitiesToDelete.clear();
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
		m_root = CreateGameobject( "root", nullptr );
		onSceneLoad.Emmit( this );
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
			if ( Save( json ) )
			{				
				Prefab::RemapGameobjectIndices( json );
				// Out to disk
				outStream << json;
			}
			outStream.close();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::Save( Json& _json ) const
	{
		// Parameters
		Json& jParameters = _json[ "parameters" ];
		{
			Serializable::SaveString( jParameters, "name", m_name );
			Serializable::SaveString( jParameters, "path", m_path );
		}

		// Gameobjects
		Json& jGameobjects = _json[ "gameobjects" ];
		{
			if ( !m_root->Save( jGameobjects ) )
			{
				return false;
			}
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::Load( const Json& _json )
	{
		//ScopedTimer timer("load scene");

		// Parameters
		const Json& jParameters = _json[ "parameters" ];
		{
			Serializable::LoadString( jParameters, "name", m_name );
			Serializable::LoadString( jParameters, "path", m_path );
		}

		// Gameobjects
		const Json& jGameobjects = _json[ "gameobjects" ];
		{
			uint64_t id;
			Serializable::LoadUInt64( jGameobjects, "gameobject_id", id );
			m_root = CreateGameobject( "root", nullptr, id );
			m_root->Load( jGameobjects );

			m_nextUniqueID = R_FindMaximumId( *m_root ) + 1;
		}
		m_instantiate->ResolveGameobjectPtr( 0 );
		m_instantiate->ResolveComponentPtr( 0 );
		return true;
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
			if ( Load( sceneJson ) )
			{
				m_path = _path;
				inStream.close();
				onSceneLoad.Emmit( this );
				return true;
			}
			else
			{
				Debug::Get() << Debug::Severity::error << "failed to load scene: " << _path << Debug::Endl();
				m_path = "";
				inStream.close();
				New();
				return false;
			}

		}
		else
		{
			Debug::Get() << Debug::Severity::error << "failed to open file " << _path << Debug::Endl();
			New();
			return false;
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