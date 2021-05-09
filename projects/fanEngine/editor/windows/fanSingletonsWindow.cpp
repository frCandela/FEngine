#include "editor/windows/fanSingletonsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "editor/fanDragnDrop.hpp"
#include "editor/gui/fanGroupsColors.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	SingletonsWindow::SingletonsWindow() :
		EditorWindow( "singletons", ImGui::IconType::Singleton16 )
	{}

	//========================================================================================================
	// draw all singletons of the ecs world
	//========================================================================================================
	void SingletonsWindow::OnGui( EcsWorld& _world )
	{
		SCOPED_PROFILE( singleton_win );
        const EditorGuiInfo& gui = _world.GetSingleton<EditorGuiInfo>();
		const std::vector< EcsSingletonInfo >& infos = _world.GetVectorSingletonInfo();
		for( const EcsSingletonInfo& info : infos )
		{
            const fan::GuiSingletonInfo& guiInfo = gui.GetSingletonInfo( info.mType );
            if( guiInfo.onGui != nullptr )
            {
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3); // moves cursor lower to center the icon
                ImGui::Icon( guiInfo.mIcon, { 16, 16 }, GroupsColors::GetColor( guiInfo.mGroup ) );
                ImGui::SameLine();
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3 );		// resets the cursor
                if( ImGui::CollapsingHeader( info.mName.c_str() ) )
                {
                    ImGui::FanBeginDragDropSourceSingleton( _world, info.mType );
                    // draws gui
                    if( guiInfo.onGui != nullptr )
                    {
                        ImGui::Indent(); ImGui::Indent();
                        guiInfo.onGui( _world, _world.GetSingleton( info.mType ) );
                        ImGui::Unindent(); ImGui::Unindent();
                    }
                }
                else
                {
                    ImGui::FanBeginDragDropSourceSingleton( _world, info.mType );
                }
			}
		}
	}
}