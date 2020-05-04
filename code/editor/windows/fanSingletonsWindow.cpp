#include "editor/windows/fanSingletonsWindow.hpp"

#include "core/time/fanProfiler.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SingletonsWindow::SingletonsWindow( EcsWorld& _world ) :
		EditorWindow( "singletons", ImGui::IconType::SINGLETON16 )
		, m_world( &_world )
	{
	}

	//================================================================================================================================
	// draw all singletons of the ecs world
	//================================================================================================================================
	void SingletonsWindow::OnGui()
	{
		SCOPED_PROFILE( singleton_win );
		
		const std::vector< SingletonComponentInfo >& info = m_world->GetVectorSingletonComponentInfo();
		for( const SingletonComponentInfo& info : info )
		{
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 3);		// moves cursor lower to center the icon
			ImGui::Icon( info.icon, { 16,16 } ); ImGui::SameLine(); 
			ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3 );		// resets the cursor
			if( ImGui::CollapsingHeader( info.name.c_str() ) )
			{
				// draws gui 
				if( info.onGui != nullptr )
				{
					info.onGui( *m_world, m_world->GetSingletonComponent( info.staticIndex ) );
				}
			}			
		}
	}
}