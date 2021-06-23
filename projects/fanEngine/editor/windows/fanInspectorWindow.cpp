#include "editor/windows/fanInspectorWindow.hpp"

#include <sstream>
#include "core/fanPath.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanDragnDrop.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void InspectorWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void InspectorWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        InspectorWindow& inspectorWindow = static_cast<InspectorWindow&>( _singleton );
        (void)inspectorWindow;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiInspectorWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        InspectorWindow& inspectorWindow = static_cast<InspectorWindow&>( _singleton );
        (void)inspectorWindow;
        SCOPED_PROFILE( inspector );

        EcsHandle handleSelected = _world.GetSingleton<EditorSelection>().mSelectedNodeHandle;
        if( handleSelected != 0 )
        {
            SceneNode& node = _world.GetComponent<SceneNode>( _world.GetEntity( handleSelected ) );
            const EcsEntity entity = _world.GetEntity( node.mHandle );

            // scene node gui
            ImGui::Icon( ImGui::Inspector16, { 16, 16 } );
            ImGui::SameLine();
            ImGui::Text( "Scene node : %s", node.mName.c_str() );

            const EditorSettings       & settings = _world.GetSingleton<EditorSettings>();
            for( const EcsComponentInfo& info : _world.GetComponentInfos() )
            {
                if( !_world.HasComponent( entity, info.mType ) ){ continue; }

                EcsComponent& component = _world.GetComponent( entity, info.mType );

                const fan::GuiComponentInfo& guiInfo = settings.GetComponentInfo( info.mType );
                if( guiInfo.onGui == nullptr ){ continue; }

                ImGui::Separator();

                // Icon
                ImGui::Icon( guiInfo.mIcon, { 16, 16 }, settings.mData->mGroupsColors.GetColor( guiInfo.mGroup ) );
                ImGui::SameLine();
                ImGui::FanBeginDragDropSourceComponent( _world, node.mHandle, info.mType, ImGuiDragDropFlags_SourceAllowNullID );
                ImGui::Text( "%s", guiInfo.mEditorName.c_str() );
                ImGui::FanBeginDragDropSourceComponent( _world, node.mHandle, info.mType, ImGuiDragDropFlags_SourceAllowNullID );

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

            InspectorWindow::NewComponentPopup( _world );
        }
    }

    //==================================================================================================================================================================================================
    // context menu when clicking "add component"
    //==================================================================================================================================================================================================
    void InspectorWindow::NewComponentPopup( EcsWorld& _world )
    {
        const EditorSettings& settings = _world.GetSingleton<EditorSettings>();

        if( ImGui::BeginPopup( "new_component" ) )
        {
            // Get components
            std::vector<EcsComponentInfo> components = _world.GetComponentInfos();
            for( int                      i          = (int)components.size() - 1; i >= 0; i-- )
            {
                const fan::GuiComponentInfo& guiInfo = settings.GetComponentInfo( components[i].mType );
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
                const fan::GuiComponentInfo& guiInfo = settings.GetComponentInfo( components[i].mType );
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
        const EditorSettings       & settings = _world.GetSingleton<EditorSettings>();
        const fan::GuiComponentInfo& guiInfo  = settings.GetComponentInfo( _info.mType );

        ImGui::Icon( guiInfo.mIcon, { 16, 16 }, settings.mData->mGroupsColors.GetColor( guiInfo.mGroup ) );
        ImGui::SameLine();
        if( ImGui::MenuItem( guiInfo.mEditorName.c_str() ) )
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