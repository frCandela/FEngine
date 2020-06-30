#include "editor/windows/fanSingletonsWindow.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanProfiler.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "editor/fanGroupsColors.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SingletonsWindow::SingletonsWindow() :
		EditorWindow( "singletons", ImGui::IconType::SINGLETON16 )
	{}

	//================================================================================================================================
	// draw all singletons of the ecs world
	//================================================================================================================================
	void SingletonsWindow::OnGui( EcsWorld& _world )
	{
		SCOPED_PROFILE( singleton_win );		

		const std::vector< EcsSingletonInfo >& infos = _world.GetVectorSingletonInfo();
		for( const EcsSingletonInfo& info : infos )
		{
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3);		// moves cursor lower to center the icon
			ImGui::Icon( info.icon, { 16,16 }, GroupsColors::GetColor( info.group ) ); ImGui::SameLine();
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3 );		// resets the cursor
			if( ImGui::CollapsingHeader( info.name.c_str() ) )
			{
				// draws gui 
				if( info.onGui != nullptr )
				{
					info.onGui( _world, _world.GetSingleton( info.type ) );
				}
			}			
		}
	}
}