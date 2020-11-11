#include "editor/windows/fanSingletonsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "scene/fanDragnDrop.hpp"
#include "editor/gui/fanGroupsColors.hpp"

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
	void SingletonsWindow::OnGui( EcsWorld& /*_world*/ )
	{
        fanAssert(false);
	    /*
		SCOPED_PROFILE( singleton_win );		

		const std::vector< EcsSingletonInfo >& infos = _world.GetVectorSingletonInfo();
		for( const EcsSingletonInfo& info : infos )
		{
            if( info.onGui != nullptr )
            {
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3); // moves cursor lower to center the icon
                ImGui::Icon( info.mIcon, { 16, 16 }, GroupsColors::GetColor( info.mGroup ) );
                ImGui::SameLine();
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3 );		// resets the cursor
                if( ImGui::CollapsingHeader( info.mName.c_str() ) )
                {
                    ImGui::FanBeginDragDropSourceSingleton( _world, info.mType );
                    // draws gui
                    if( info.onGui != nullptr )
                    {
                        ImGui::Indent(); ImGui::Indent();
                        info.onGui( _world, _world.GetSingleton( info.mType ) );
                        ImGui::Unindent(); ImGui::Unindent();
                    }
                }
                else
                {
                    ImGui::FanBeginDragDropSourceSingleton( _world, info.mType );
                }
			}
		}*/
	}
}