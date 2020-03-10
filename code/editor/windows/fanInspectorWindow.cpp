#include "editor/windows/fanInspectorWindow.hpp"

#include "editor/components/fanFPSCamera.hpp"
#include "game/imgui/fanDragnDrop.hpp"
#include "scene/ecs/fanEntityWorld.hpp"
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
			EntityWorld& world = scene.GetEntityWorld();
			const EntityID id = world.GetEntityID( node.entityHandle );
			Entity& entity = world.GetEntity( id );

			// gameobject gui
 			ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
			ImGui::Text( "Scene node : %s", node.name.c_str() );

			for( int componentIndex = 0; componentIndex < entity.componentCount; componentIndex++ )
			{
				ecComponent& component = *entity.components[componentIndex];
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
					world.RemoveComponent( id, component.GetIndex() );
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

			EntityWorld& world = m_sceneNodeSelected->scene->GetEntityWorld();
			EntityID id = world.GetEntityID( m_sceneNodeSelected->entityHandle );
			if( !world.HasComponent( id, _info.index ) )
			{
				world.AddComponent( id, _info.index );
			}

			
			ImGui::CloseCurrentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup()
	{
		using Path = std::filesystem::path;

		EntityWorld& world = m_sceneNodeSelected->scene->GetEntityWorld();

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