#include "editor/windows/fanInspectorWindow.hpp"

#include <sstream>
#include <core/fanPath.hpp>
#include "editor/fanDragnDrop.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "core/time/fanProfiler.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/gui/fanGroupsColors.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    InspectorWindow::InspectorWindow() :
            EditorWindow( "inspector", ImGui::IconType::Inspector16 ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void InspectorWindow::OnGui( EcsWorld& _world )
    {
        SCOPED_PROFILE( inspector );

        EcsHandle handleSelected = _world.GetSingleton<EditorSelection>().mSelectedNodeHandle;
        if( handleSelected != 0 )
        {
            SceneNode& node = _world.GetComponent<SceneNode>( _world.GetEntity( handleSelected ) );
            const EcsEntity entity = _world.GetEntity( node.mHandle );

            // scene node gui
            ImGui::Icon( GetIconType(), { 16, 16 } );
            ImGui::SameLine();
            ImGui::Text( "Scene node : %s", node.mName.c_str() );

            const EditorGuiInfo        & gui = _world.GetSingleton<EditorGuiInfo>();
            for( const EcsComponentInfo& info : _world.GetComponentInfos() )
            {
                if( !_world.HasComponent( entity, info.mType ) ){ continue; }

                EcsComponent& component = _world.GetComponent( entity, info.mType );

                const fan::GuiComponentInfo& guiInfo = gui.GetComponentInfo( info.mType );
                if( guiInfo.onGui == nullptr ){ continue; }

                ImGui::Separator();

                // Icon
                ImGui::Icon( guiInfo.mIcon, { 16, 16 }, GroupsColors::GetColor( guiInfo.mGroup ) );
                ImGui::SameLine();
                ImGui::FanBeginDragDropSourceComponent( _world,
                                                        node.mHandle,
                                                        info.mType,
                                                        ImGuiDragDropFlags_SourceAllowNullID );
                ImGui::Text( "%s", info.mName.c_str() );
                ImGui::FanBeginDragDropSourceComponent( _world,
                                                        node.mHandle,
                                                        info.mType,
                                                        ImGuiDragDropFlags_SourceAllowNullID );

                // Delete button
                std::stringstream ss;
                ss << "X" << "##" << info.mName;    // make unique id
                ImGui::SameLine( ImGui::GetWindowWidth() - 40 );
                if( ImGui::Button( ss.str().c_str() ) )
                {
                    _world.RemoveComponent( entity, info.mType );
                }
                    // Draw component
                else
                {
                    guiInfo.onGui( _world, entity, component );
                }
            }
            ImGui::Separator();
            //Add component button
            if( ImGui::Button( "Add component" ) )
            {
                ImGui::OpenPopup( "new_component" );
            }

            NewComponentPopup( _world );
        }
    }

    //==================================================================================================================================================================================================
    // context menu when clicking "add component"
    //==================================================================================================================================================================================================
    void InspectorWindow::NewComponentPopup( EcsWorld& _world )
    {
        const EditorGuiInfo& gui = _world.GetSingleton<EditorGuiInfo>();

        if( ImGui::BeginPopup( "new_component" ) )
        {
            // Get components
            std::vector<EcsComponentInfo> components = _world.GetComponentInfos();
            for( int                      i          = (int)components.size() - 1; i >= 0; i-- )
            {
                const fan::GuiComponentInfo& guiInfo = gui.GetComponentInfo( components[i].mType );
                if( std::string( guiInfo.mEditorPath ).empty() )
                {
                    components.erase( components.begin() + i );
                }
            }

            // Get components paths
            std::vector<std::string> componentsPath;
            componentsPath.reserve( components.size() );
            for( int i = 0; i < (int)components.size(); i++ )
            {
                const fan::GuiComponentInfo& guiInfo = gui.GetComponentInfo( components[i].mType );
                componentsPath.push_back( guiInfo.mEditorPath );
            }

            // Sort components paths & remove duplicates
            std::set<std::string> pathSet( componentsPath.begin(), componentsPath.end() );
            pathSet.erase( "/" );
            std::vector<std::string> sortedPath( pathSet.begin(), pathSet.end() );

            R_DrawHierarchy( sortedPath, "", _world, components, componentsPath );

            ImGui::EndPopup();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void InspectorWindow::R_DrawHierarchy( std::vector<std::string> _current,
                                           std::string _fullPath,
                                           EcsWorld& _world,
                                           const std::vector<EcsComponentInfo>& _components,
                                           const std::vector<std::string>& _componentsPath )
    {
        // Draw all the folders
        while( !_current.empty() )
        {
            std::vector<std::string> next;
            std::string              rootDir = SplitPaths( _current, next );

            std::string dirDisplayName = rootDir;
            if( dirDisplayName.ends_with( '/' ) ){ dirDisplayName.pop_back(); }
            if( ImGui::BeginMenu( dirDisplayName.c_str() ) )
            {
                R_DrawHierarchy( next, _fullPath + rootDir, _world, _components, _componentsPath );
                ImGui::EndMenu();
            }
        }

        // Draw the components
        if( _fullPath.empty() ){ _fullPath = '/'; }
        for( int componentIndex = 0; componentIndex < (int)_components.size(); componentIndex++ )
        {
            if( _componentsPath[componentIndex] == _fullPath )
            {
                NewComponentItem( _world, _components[componentIndex] );
            }
        }
    }

    //==================================================================================================================================================================================================
    // splits a list of paths.
    // _current half will have the same root folder
    // _next half  will have some other root folder
    // returs the _current root folder
    // ex: SplitPaths({"dir1/a","dir1/b","dir2/c"},{}) = {"dir1/a","dir1/b"},{"dir2/c"} : returns "dir1"
    //==================================================================================================================================================================================================
    std::string InspectorWindow::SplitPaths( std::vector<std::string>& _current, std::vector<std::string>& _next )
    {
        fanAssert( _next.empty() );
        std::string first = _current[0];
        std::string rootDir( first.begin(), first.begin() + first.find_first_of( '/' ) + 1 );
        while( !_current.empty() && _current[0].compare( 0, rootDir.size(), rootDir ) == 0 )
        {
            std::string nextPath = std::string( _current[0].begin() + rootDir.size(), _current[0].end() );
            if( !nextPath.empty() )
            {
                _next.push_back( nextPath );
            }
            _current.erase( _current.begin() );
        }
        return rootDir;
    }

    //==================================================================================================================================================================================================
    // menu item in the NewComponentPopup
    //==================================================================================================================================================================================================
    void InspectorWindow::NewComponentItem( EcsWorld& _world, const EcsComponentInfo& _info )
    {
        const EditorGuiInfo        & gui     = _world.GetSingleton<EditorGuiInfo>();
        const fan::GuiComponentInfo& guiInfo = gui.GetComponentInfo( _info.mType );

        ImGui::Icon( guiInfo.mIcon, { 16, 16 }, GroupsColors::GetColor( guiInfo.mGroup ) );
        ImGui::SameLine();
        if( ImGui::MenuItem( _info.mName.c_str() ) )
        {
            // Create new EcsComponent
            EcsHandle handleSelected = _world.GetSingleton<EditorSelection>().mSelectedNodeHandle;
            EcsEntity entity         = _world.GetEntity( handleSelected );
            if( !_world.HasComponent( entity, _info.mType ) )
            {
                _world.AddComponent( entity, _info.mType );
            }
            ImGui::CloseCurrentPopup();
        }
    }
}