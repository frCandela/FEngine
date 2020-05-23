#include "editor/windows/fanInspectorWindow.hpp"

#include <sstream>
#include "scene/fanDragnDrop.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanSignal.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow( EcsWorld& _world ) :
		EditorWindow( "inspector", ImGui::IconType::INSPECTOR16 ), 
		m_world( _world )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnSceneNodeSelected( SceneNode* _node ) 
	{
		m_handleNodeSelected = _node == nullptr ? 0 : _node->handle; 
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnGui()
	{
		SCOPED_PROFILE( inspector );

		if( m_handleNodeSelected != 0 )
		{
			SceneNode& node = m_world.GetComponent<SceneNode>( m_world.GetEntity(m_handleNodeSelected) );
			Scene& scene = *node.scene;
			EcsWorld& world = *scene.world;
			const EcsEntity entity = world.GetEntity( node.handle );

			// scene node gui
 			ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
			ImGui::Text( "Scene node : %s", node.name.c_str() );

			
			for( const EcsComponentInfo& info : world.GetVectorComponentInfo() )
			{
				if( !world.HasComponent( entity, info.type ) ) { continue; }

				EcsComponent& component = world.GetComponent( entity, info.type );

				if( info.onGui == nullptr ) { continue; }

 				ImGui::Separator();
				 
 				// Icon
				ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine();
				ImGui::FanBeginDragDropSourceComponent( node, component, ImGuiDragDropFlags_SourceAllowNullID );
				ImGui::Text( "%s", info.name.c_str() );
 				ImGui::FanBeginDragDropSourceComponent( node, component, ImGuiDragDropFlags_SourceAllowNullID );

 				// Delete button	
				std::stringstream ss;
				ss << "X" << "##" << info.name;	// make unique id
				ImGui::SameLine( ImGui::GetWindowWidth() - 40 );
				if( ImGui::Button( ss.str().c_str() ) )
				{
					world.RemoveComponent( entity, info.type );
				}
 				// Draw component
				else
				{
					info.onGui( world, entity, component );
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
	// menu item in the NewComponentPopup
	//================================================================================================================================
	void InspectorWindow::NewComponentItem( const EcsComponentInfo& _info )
	{
		ImGui::Icon( _info.icon, { 16,16 } ); ImGui::SameLine();
		if( ImGui::MenuItem( _info.name.c_str() ) )
		{
			// Create new EcsComponent 
			EcsEntity entity = m_world.GetEntity( m_handleNodeSelected );
			if( !m_world.HasComponent( entity, _info.type ) )
			{
				m_world.AddComponent( entity, _info.type );
			}			
			ImGui::CloseCurrentPopup();
		}
	}

	//================================================================================================================================
	// context menu when clicking "add component"
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup()
	{
		using Path = std::filesystem::path;
		if( ImGui::BeginPopup( "new_component" ) )
		{
			// Get components and remove components with an empty path
			std::vector< EcsComponentInfo > components = m_world.GetVectorComponentInfo();			
			for( int i = (int)components.size() - 1; i >= 0; i-- )
			{
				if( std::string( components[i].editorPath ).empty() )
				{
					components.erase( components.begin() + i );
				}
			}

 			// Get components paths
			std::vector<Path> componentsPath;
			componentsPath.reserve( components.size() );
			for( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				componentsPath.push_back(  components[componentIndex].editorPath );
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
					NewComponentItem( components[componentIndex] ); 
				}
			}

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	// recursively draws all components available to add
	// called from the NewComponentPopup
	//================================================================================================================================
	void InspectorWindow::R_NewComponentPopup( std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector< EcsComponentInfo >& _components, const std::vector<std::filesystem::path>& _componentsPath )
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
					NewComponentItem( _components[componentIndex] );
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