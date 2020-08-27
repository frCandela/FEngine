#pragma once

#include "ecs/fanEcsTypes.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct GroupsColors
	{
		static constexpr const int sCount = static_cast<int>( EngineGroups::Count );

		static ImVec4 sColors[ sCount ];

		static ImVec4 GetColor( const EngineGroups _group )
		{
			return sColors[static_cast<int>( _group )];
		}
	};
}