#include "editor/windows/fanInspectorWindow.hpp"

#include "editor/components/fanFPSCamera.hpp"
#include "game/imgui/fanDragnDrop.hpp"
#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanSphereShape2.hpp"
#include "scene/ecs/components/fanBoxShape2.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanRigidbody2.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/fanGameobject.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanSignal.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"
#include "scene/ecs/fanEcsWorld.hpp"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow() :
		EditorWindow( "inspector", ImGui::IconType::INSPECTOR16 )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnGui()
	{
		SCOPED_PROFILE( inspector );

		if( m_sceneNodeSelected != nullptr )
		{
			SceneNode& node = *m_sceneNodeSelected;
			Scene& scene = *node.scene;
			EcsWorld& world = scene.GetWorld();
			const EntityID entityID = world.GetEntityID( node.entityHandle );

			// gameobject gui
 			ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
			ImGui::Text( "Scene node : %s", node.name.c_str() );

			for( int componentIndex = 0; componentIndex < world.GetComponentCount( entityID ); componentIndex++ )
			{
				ecComponent& component = world.GetComponentAt( entityID, componentIndex );
				const ComponentInfo& info = world.GetComponentInfo( component.GetIndex() );

				if( std::string(info.editorPath).empty() ) { continue; }

 				ImGui::Separator();
				 
 				// Icon
				ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
				ImGui::Text( "%s", info.name.c_str() );
 				ImGui::FanBeginDragDropSourceEcComponent( component, info, ImGuiDragDropFlags_SourceAllowNullID );

 				// Delete button	
				std::stringstream ss;
				ss << "X" << "##" << info.name;	// make unique id
				ImGui::SameLine( ImGui::GetWindowWidth() - 40 );
				if( ImGui::Button( ss.str().c_str() ) )
				{
					// if component is a rigidbody, unregisters from physics world
					bool rigidbodyNeedsUpdate = PhysicsWorld::IsPhysicsType( world, component );
					if( world.IsType<Rigidbody2>( component ) )
					{
						Rigidbody2& rb = static_cast<Rigidbody2&>( component );
						PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
						physicsWorld.dynamicsWorld->removeRigidBody( &rb.rigidbody );
						rigidbodyNeedsUpdate = false;
					}					

					world.RemoveComponent( entityID, component.GetIndex() );
					
					if( rigidbodyNeedsUpdate ) { UpdateEntityRigidbody( world, entityID ); }					
				}
 				// Draw component
				else if( info.onGui != nullptr )
				{
					info.onGui( component );
				} 
			}
			ImGui::Separator();
			//Add component button
			if( ImGui::Button( "Add component" ) )
				ImGui::OpenPopup( "new_component" );

			NewComponentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  InspectorWindow::UpdateEntityRigidbody( EcsWorld& _world, EntityID _entityID )
	{		
		if( _world.HasComponent<Rigidbody2>( _entityID ) )
		{
			Rigidbody2& rb = _world.GetComponent<Rigidbody2>( _entityID );

			// find a collision shape
			btCollisionShape* shape = nullptr;
			if( _world.HasComponent<SphereShape2>( _entityID ) )
			{
				shape = &_world.GetComponent<SphereShape2>( _entityID ).sphereShape;
			}
			else if( _world.HasComponent<BoxShape2>( _entityID ) )
			{
				shape = &_world.GetComponent<BoxShape2>( _entityID ).boxShape;
			}

			// find a motion state
			btDefaultMotionState* motionState = nullptr;
			if( _world.HasComponent<MotionState>( _entityID ) )
			{
				motionState = &_world.GetComponent<MotionState>( _entityID ).motionState;
			}

			// reset the rigidbody
			PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
			physicsWorld.dynamicsWorld->removeRigidBody( &rb.rigidbody );
			rb.SetMotionState( motionState );
			rb.SetCollisionShape( shape );
			physicsWorld.dynamicsWorld->addRigidBody( &rb.rigidbody );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentItem( const ComponentInfo& _info )
	{
		ImGui::Icon( _info.icon, { 16,16 } ); ImGui::SameLine();
		if( ImGui::MenuItem( _info.name.c_str() ) )
		{
			// Create new Component 
			EcsWorld& world = m_sceneNodeSelected->scene->GetWorld();
			EntityID entityID = world.GetEntityID( m_sceneNodeSelected->entityHandle );
			if( !world.HasComponent( entityID, _info.index ) )
			{
				ecComponent& component = world.AddComponent( entityID, _info.index );
				if( PhysicsWorld::IsPhysicsType( world, component ) )
				{
					UpdateEntityRigidbody( world, entityID );
				}
				
			}			
			ImGui::CloseCurrentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup()
	{
		using Path = std::filesystem::path;

		EcsWorld& world = m_sceneNodeSelected->scene->GetWorld();

		if( ImGui::BeginPopup( "new_component" ) )
		{
			// Get components and remove components with an empty path
			std::vector< const ComponentInfo*> components;
			world.GetVectorComponentInfo( components );			
			for( int i = (int)components.size() - 1; i >= 0; i-- )
			{
				if( std::string( components[i]->editorPath ).empty() )
				{
					components.erase( components.begin() + i );
				}
			}


 			// Get components paths
			std::vector<Path> componentsPath;
			componentsPath.reserve( components.size() );
			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				componentsPath.push_back(  components[componentIndex]->editorPath );
			}

			// Sort components paths
			std::set< Path > componentsPathSet;
			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				componentsPathSet.insert( componentsPath[componentIndex] );
			} componentsPathSet.erase( "" );

			// Draw menus recursively
			std::set< Path >::iterator it = componentsPathSet.begin();
			while( it != componentsPathSet.end() )
			{
				R_NewComponentPopup( componentsPathSet, it, components, componentsPath );
				++it;
			}

			// Draw menu items for components at path "/"
			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				if( componentsPath[componentIndex] == "/" ) 
				{ 
					NewComponentItem( *components[componentIndex] ); 
				}
			}

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::R_NewComponentPopup( std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector< const ComponentInfo*>& _components, const std::vector<std::filesystem::path>& _componentsPath )
	{
		std::filesystem::path rootPath = *_current;

		std::string name = rootPath.string();

		// Replace back slashes with forward slashes
		for( int charIndex = 0; charIndex < name.size(); charIndex++ )
		{
			if( name[charIndex] == '\\' ) { name[charIndex] = '/'; }
		}

		// Remove trailing slashes
		while( !name.empty() && name[name.size() - 1] == '/' ) {
			name.erase( name.end() - 1 ); 
		} 
		size_t lastSlash = name.find_last_of( "/" );
		if( lastSlash != std::string::npos )
		{
			name.erase( name.begin(), name.begin() + lastSlash + 1 );
		}

		if( name != "" && ImGui::BeginMenu( name.c_str() ) )
		{
			// draw next menu paths
			std::set< std::filesystem::path >::iterator next = _current;
			next++;
			while( next != _componentsPathSet.end() && next->string().find( rootPath.string() ) == 0 )
			{
				++_current;
				R_NewComponentPopup( _componentsPathSet, _current, _components, _componentsPath );
				++next;
			}

			// draw menu items (components)
			for( int componentIndex = 0; componentIndex < _components.size(); componentIndex++ )
			{
				if( _componentsPath[componentIndex] == rootPath.string() )
				{
					NewComponentItem( *_components[componentIndex] );
				}
			}
			ImGui::EndMenu();
		}
		else
		{
			// If the menu is not open, consumes all the following expressions under its path
			std::set< std::filesystem::path >::iterator next = _current;
			++next;
			while( next != _componentsPathSet.end() && next->string().find( rootPath.string() ) == 0 )
			{
				++_current;
				++next;
			}
		}
	}
}