#include "editor/windows/fanInspectorWindow.hpp"

#include "editor/components/fanFPSCamera.hpp"
#include "game/imgui/fanDragnDrop.hpp"
#include "scene/ecs/fanEcsWorld.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
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

#include "scene/ecs/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/ecs/components/fanRigidbody2.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanSphereShape2.hpp"
#include "scene/ecs/components/fanBoxShape2.hpp"

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
					world.RemoveComponent( entityID, component.GetIndex() );
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
	void InspectorWindow::NewComponentItem( const ComponentInfo& _info )
	{
		ImGui::Icon( _info.icon, { 16,16 } ); ImGui::SameLine();
		if( ImGui::MenuItem( _info.name.c_str() ) )
		{
			// Create new Component 

			EcsWorld& world = m_sceneNodeSelected->scene->GetWorld();
			EntityID id = world.GetEntityID( m_sceneNodeSelected->entityHandle );
			if( !world.HasComponent( id, _info.index ) )
			{
				ecComponent& component = world.AddComponent( id, _info.index );
				RegisterPhysics( world, id, component );	

			}			
			ImGui::CloseCurrentPopup();
		}
	}

	//================================================================================================================================
	// registers rigidbody, colliders and motions state into each others and into the physics world when created from the editor
	//================================================================================================================================
	void InspectorWindow::RegisterPhysics( EcsWorld& _world, EntityID _entity, ecComponent& _component )
	{
		
		if( _world.IsType<Rigidbody2>( _component ) )
		{
			// Rigidbody
			PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
			Rigidbody2& rb = static_cast<Rigidbody2&>( _component );

			// find collision shape
			btCollisionShape* shape = nullptr;
			if( _world.HasComponent< SphereShape2 >( _entity ) ) 
			{ 
				shape = &_world.GetComponent<SphereShape2>( _entity ).sphereShape; 
			}
			else if( _world.HasComponent< BoxShape2 >( _entity ) )
			{
				shape = &_world.GetComponent<BoxShape2>( _entity ).boxShape;
			}
			if( shape !=  nullptr )
			{
				rb.SetCollisionShape( shape );
			}

			// find motion state
			if( _world.HasComponent< MotionState >( _entity ) )
			{
				MotionState& motionState = _world.GetComponent<MotionState>( _entity );
				rb.rigidbody.setMotionState( &motionState.motionState );
			}
			
			physicsWorld.dynamicsWorld->addRigidBody( &rb.rigidbody );
		}		
		else if( _world.HasComponent< Rigidbody2 >( _entity ) )
		{
			Rigidbody2& rb = _world.GetComponent<Rigidbody2>( _entity );
			if( _world.IsType<MotionState>( _component ) )
			{
				// motion state
				MotionState& motionState = static_cast<MotionState&>( _component );
				
				rb.rigidbody.setMotionState( &motionState.motionState );
			}
			else
			{
				// collider
				btCollisionShape* shape = nullptr;
				if( _world.IsType<SphereShape2>( _component ) )
				{
					shape = &static_cast<SphereShape2&>( _component ).sphereShape;
				}
				else if( _world.IsType<BoxShape2>( _component ) )
				{
					shape = &static_cast<BoxShape2&>( _component ).boxShape;
				}
				if( shape != nullptr )
				{
					rb.SetCollisionShape( shape );
				}
			}
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
			// Get components 
			std::vector< const ComponentInfo*> components;
			world.GetVectorComponentInfo( components );

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

			// Draw menu items for components at path ""
			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				if( componentsPath[componentIndex] == "" ) 
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
		while( name[name.size() - 1] == '/' ) { name.erase( name.end() - 1 ); } 
		size_t lastSlash = name.find_last_of( "/" );
		if( lastSlash != std::string::npos )
		{
			name.erase( name.begin(), name.begin() + lastSlash + 1 );
		}

		if( ImGui::BeginMenu( name.c_str() ) )
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