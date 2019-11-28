#include "fanGlobalIncludes.h"
#include "editor/windows/fanInspectorWindow.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanMeshRenderer.h"
#include "scene/components/fanMaterial.h"
#include "editor/components/fanFPSCamera.h"
#include "editor/fanDragnDrop.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "core/time/fanProfiler.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow() :
		EditorWindow("inspector", ImGui::IconType::INSPECTOR16 )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnGui() {
		SCOPED_PROFILE( inspector )
		if ( m_gameobjectSelected != nullptr)
		{
			// gameobject gui
			ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
			m_gameobjectSelected->OnGui();
			int componentCount = 0;			
			const std::vector<Component*> & components = m_gameobjectSelected->GetComponents();
			for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
				Component * component = components[componentIndex];

				ImGui::Separator();

				// Icon
				ImGui::Icon( component->GetIcon(), { 16,16 } ); ImGui::SameLine();
				ImGui::FanBeginDragDropSourceComponent( component,  ImGuiDragDropFlags_SourceAllowNullID);

				// Actor "enable" checkbox
				if (component->IsActor()) {	// TODO : use type info when type info deals with inheritance
					ImGui::PushID((int*)component);
					Actor * actor = static_cast<Actor*>(component);
					bool enabled = actor->IsEnabled();
					if (ImGui::Checkbox("", &enabled)) {
						actor->SetEnabled(enabled);
					}
					ImGui::SameLine();
					ImGui::PopID();
				}

				// Delete button	
				ImGui::Text( component->GetName());
				ImGui::FanBeginDragDropSourceComponent( component,  ImGuiDragDropFlags_SourceAllowNullID);

				if (component->IsRemovable()) {
					std::stringstream ss;
					ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
					ImGui::SameLine(ImGui::GetWindowWidth() - 40);
					if (ImGui::Button(ss.str().c_str())) {
						m_gameobjectSelected->RemoveComponent(component);
						component = nullptr;
					}
				}
				
				// Draw component
				if (component != nullptr) {
					component->OnGui();
				}
			}			
			ImGui::Separator();
			//Add component button
			if (ImGui::Button("Add component"))
				ImGui::OpenPopup("New component");

			NewComponentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentItem( const Component* _component )
	{
		ImGui::Icon( _component->GetIcon(), { 16,16 } ); ImGui::SameLine();
		if ( ImGui::MenuItem( _component->GetName() ) )
		{
			// Create new Component 
			m_gameobjectSelected->AddComponent( _component->GetType() );
			ImGui::CloseCurrentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup() {
		using Path = std::fs::path;

		
		if (ImGui::BeginPopup("New component"))
		{
			// Get components 
			std::vector< const Component *> components;
			for ( const void * component : TypeInfo::Get().GetInstancesWithFlags( TypeInfo::EDITOR_COMPONENT ) )
			{
				components.push_back( static_cast<const Component *>(component));
			}

			// Get components paths
			std::vector<Path> componentsPath;
			componentsPath.reserve(components.size());
			for ( int componentIndex = 0; componentIndex < components.size(); componentIndex++ ) { 
				componentsPath.push_back(TypeInfo::Get().GetPath( components[componentIndex]->GetType() )); 
			}

			// Sort components paths
			std::set< Path > componentsPathSet;
			for ( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				componentsPathSet.insert( componentsPath[componentIndex] ); 
			} componentsPathSet.erase("");

			// Draw menus recursively
			std::set< Path >::iterator it = componentsPathSet.begin();
			while ( it != componentsPathSet.end() )
			{
				R_NewComponentPopup( componentsPathSet, it, components, componentsPath );
				++it;
			}

			// Draw menu items for components at path ""
			for ( int componentIndex = 0; componentIndex < components.size(); componentIndex++ )
			{
				if ( componentsPath[componentIndex] == "" )	{ NewComponentItem( components[componentIndex] );	}
			}

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::R_NewComponentPopup( std::set< std::fs::path >& _componentsPathSet, std::set< std::fs::path >::iterator&  _current, const std::vector< const Component *>& _components, const std::vector<std::fs::path>& _componentsPath )
	{
		std::fs::path rootPath = *_current;

		std::string name = rootPath.string();

		// Replace back slashes with forward slashes
		for ( int charIndex = 0; charIndex < name.size(); charIndex++ )
		{
			if ( name[charIndex] == '\\' ) { name[charIndex] = '/'; }
		}

		while ( name[name.size() - 1] == '/' ) { name.erase( name.end() - 1 ); } // Remove trailing slashes
		size_t lastSlash = name.find_last_of( "/" );
		if ( lastSlash != std::string::npos )
		{
			name.erase( name.begin(), name.begin() + lastSlash + 1 );
		}

		if ( ImGui::BeginMenu( name.c_str() ) )
		{
			// draw next menu paths
			std::set< std::fs::path >::iterator next = _current;
			next++;
			while ( next != _componentsPathSet.end() && next->string().find( rootPath.string() ) == 0 )
			{
				++_current;
				R_NewComponentPopup( _componentsPathSet, _current, _components, _componentsPath );
				++next;
			}

			// draw menu items (components)
			for ( int componentIndex = 0; componentIndex < _components.size(); componentIndex++ )
			{
				if ( _componentsPath[componentIndex] == rootPath.string() )
				{
					const Component * component = _components[componentIndex];
					NewComponentItem( component );
				}
			}
			ImGui::EndMenu();
		}
		else
		{
			// If the menu is not open, consumes all the following expressions under its path
			std::set< std::fs::path >::iterator next = _current;
			++next;
			while ( next != _componentsPathSet.end() && next->string().find( rootPath.string() ) == 0 )
			{
				++_current;
				++next;
			}
		}
	}
}