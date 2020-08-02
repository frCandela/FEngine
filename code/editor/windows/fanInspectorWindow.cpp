#include "editor/windows/fanInspectorWindow.hpp"

#include <sstream>
#include "scene/fanDragnDrop.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"
#include "ecs/fanSignal.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/resources/fanMesh.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/fanGroupsColors.hpp"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow() :
		EditorWindow( "inspector", ImGui::IconType::INSPECTOR16 )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnGui( EcsWorld& _world )
	{
		SCOPED_PROFILE( inspector );

		EcsHandle handleSelected = _world.GetSingleton<EditorSelection>().m_selectedNodeHandle;
		if( handleSelected != 0 )
		{
			SceneNode& node = _world.GetComponent<SceneNode>( _world.GetEntity( handleSelected ) );
			const EcsEntity entity = _world.GetEntity( node.handle );

			// scene node gui
 			ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
			ImGui::Text( "Scene node : %s", node.name.c_str() );

			
			for( const EcsComponentInfo& info : _world.GetComponentInfos() )
			{
				if( !_world.HasComponent( entity, info.type ) ) { continue; }

				EcsComponent& component = _world.GetComponent( entity, info.type );

				if( info.onGui == nullptr ) { continue; }

 				ImGui::Separator();
				 
 				// Icon
				ImGui::Icon( info.icon, { 16,16 }, GroupsColors::GetColor( info.group ) ); ImGui::SameLine();
				ImGui::FanBeginDragDropSourceComponent( _world, node.handle, info.type, ImGuiDragDropFlags_SourceAllowNullID );
				ImGui::Text( "%s", info.name.c_str() );
 				ImGui::FanBeginDragDropSourceComponent( _world, node.handle, info.type, ImGuiDragDropFlags_SourceAllowNullID );

 				// Delete button	
				std::stringstream ss;
				ss << "X" << "##" << info.name;	// make unique id
				ImGui::SameLine( ImGui::GetWindowWidth() - 40 );
				if( ImGui::Button( ss.str().c_str() ) )
				{
					_world.RemoveComponent( entity, info.type );
				}
 				// Draw component
				else
				{
					info.onGui( _world, entity, component );
				} 
			}
			ImGui::Separator();
			//Add component button
			if( ImGui::Button( "Add component" ) )
				ImGui::OpenPopup( "new_component" );

			NewComponentPopup( _world );
		}
	}

	//================================================================================================================================
	// menu item in the NewComponentPopup
	//================================================================================================================================
	void InspectorWindow::NewComponentItem( EcsWorld& _world, const EcsComponentInfo& _info )
	{
		ImGui::Icon( _info.icon, { 16,16 }, GroupsColors::GetColor( _info.group ) ); ImGui::SameLine();
		if( ImGui::MenuItem( _info.name.c_str() ) )
		{
			// Create new EcsComponent 
			EcsHandle handleSelected = _world.GetSingleton<EditorSelection>().m_selectedNodeHandle;
			EcsEntity entity = _world.GetEntity( handleSelected );
			if( !_world.HasComponent( entity, _info.type ) )
			{
				_world.AddComponent( entity, _info.type );
			}			
			ImGui::CloseCurrentPopup();
		}
	}

	//================================================================================================================================
	// context menu when clicking "add component"
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup( EcsWorld& _world )
	{
		using Path = std::filesystem::path;
		if( ImGui::BeginPopup( "new_component" ) )
		{
			// Get components and remove components with an empty path
			std::vector< EcsComponentInfo > components = _world.GetComponentInfos();			
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
			for( int componentIndex = 0; componentIndex < (int)components.size(); componentIndex++ )
			{
				componentsPath.push_back(  components[componentIndex].editorPath );
			}

			// Sort components paths
			std::set< Path > componentsPathSet;
			for( int componentIndex = 0; componentIndex < (int)components.size(); componentIndex++ )
			{
				componentsPathSet.insert( componentsPath[componentIndex] );
			} componentsPathSet.erase( "" );

			// Draw menus recursively
			std::set< Path >::iterator it = componentsPathSet.begin();
			while( it != componentsPathSet.end() )
			{
				R_NewComponentPopup( _world, componentsPathSet, it, components, componentsPath );
				++it;
			}

			// Draw menu items for components at path "/"
			for( int componentIndex = 0; componentIndex < (int)components.size(); componentIndex++ )
			{
				if( componentsPath[componentIndex] == "/" ) 
				{ 
					NewComponentItem( _world, components[componentIndex] );
				}
			}

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	// recursively draws all components available to add
	// called from the NewComponentPopup
	//================================================================================================================================
	void InspectorWindow::R_NewComponentPopup( EcsWorld& _world, std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector< EcsComponentInfo >& _components, const std::vector<std::filesystem::path>& _componentsPath )
	{
		std::filesystem::path rootPath = *_current;

		std::string name = rootPath.string();

		// Replace back slashes with forward slashes
		for( int charIndex = 0; charIndex < (int)name.size(); charIndex++ )
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
				R_NewComponentPopup( _world, _componentsPathSet, _current, _components, _componentsPath );
				++next;
			}

			// draw menu items (components)
			for( int componentIndex = 0; componentIndex < (int)_components.size(); componentIndex++ )
			{
				if( _componentsPath[componentIndex] == rootPath.string() )
				{
					NewComponentItem( _world, _components[componentIndex] );
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