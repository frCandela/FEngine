#pragma once

#include "ecs/fanEcsTypes.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct GroupsColors
	{
		static constexpr const int s_count = static_cast<int>( EngineGroups::Count );

		static ImVec4 s_colors[ s_count ];

		static ImVec4 GetColor( const EngineGroups _group )
		{
			return s_colors[static_cast<int>( _group )];
		}
	};
}