#pragma once

#include "imgui/imgui.h"

namespace ImGui
{
	//================================================================================================================================
	// utility for displaying imgui icons
	//================================================================================================================================
	enum IconType : int
	{
		NONE = 0,

		// editor windows
		IMGUI16,
		RENDERER16,
		SCENE16,
		INSPECTOR16,
		CONSOLE16,
		ECS16,
		PROFILER16,
		PREFERENCES16,
		SINGLETON16,

		// special
		GAMEOBJECT16,
		PREFAB16,
		MESH16,
		LOCK_OPEN16,
		LOCK_CLOSED16,
		NETWORK16,

		// special
		PLAY16,
		PAUSE16,
		STEP16,
		STOP16,
        CHECK_NEUTRAL16,
        CHECK_FAILED16,
        CHECK_SUCCESS16,

		// scene components
		IMAGE16,
		CAMERA16,
		TRANSFORM16,
		PARTICLES16,
		DIR_LIGHT16,
		POINT_LIGHT16,
		MATERIAL16,
		MESH_RENDERER16,
		RIGIDBODY16,
		CUBE_SHAPE16,
		SPHERE_SHAPE16,
		UI_MESH_RENDERER16,
		UI_TRANSFORM16,
		UI_PROGRESS_BAR16,
		FOLLOW_TRANSFORM,
		INPUT16,
		FOLLOW_TRANSFORM_UI16,
		EXPIRATION16,

		// singletons
		TIME16,
		SPAWN16,
		CONNECTION16,
		SOCKET16,
		PTR16,
		GRID16,
		SELECTION16,
		GIZMOS16,
		COPY_PASTE16,
		LINK16,
		CLIENT_NET16,
		RENDER_DEBUG16,

		// generics	
		SOLAR_SYSTEM16,
		HEART16,
		ENERGY16,
		JOYSTICK16,
		SPACE_SHIP16,
		PLANET16,
		SUN16,
		SUN_RED16,

		// network
		RELIABILITY_LAYER16,
		GAME_DATA16,
		NUM_ICONS
	};

	//================================================================================================================================
	//================================================================================================================================
	struct IconImpl
	{
		struct IconData
		{
			ImVec2 uv0;
			ImVec2 uv1;
		};
		static const float s_iconImageSize;
		static const IconData s_iconsList[];
	};

	//================================================================================================================================
	//================================================================================================================================
	inline void Icon( const IconType _id,
					  const ImVec2& _size,
					  const ImVec4& _tintColor = ImVec4( 1, 1, 1, 1 ),
					  const ImVec4& _borderColor = ImVec4( 0, 0, 0, 0 ) )
	{
		ImGui::Image( ( void* ) 42, _size, IconImpl::s_iconsList[ _id ].uv0, IconImpl::s_iconsList[ _id ].uv1, _tintColor, _borderColor );
	}

	//================================================================================================================================
	//================================================================================================================================
	inline bool ButtonIcon( const IconType _id,
							const ImVec2& _size,
							const int _framePadding = -1,
							const ImVec4& _bgColor = ImVec4( 0, 0, 0, 0 ),
							const ImVec4& _tintColor = ImVec4( 1, 1, 1, 1 ) )
	{
		ImGui::PushID( _id );
		bool result = ImGui::ImageButton( ( void* ) 42, _size, IconImpl::s_iconsList[ _id ].uv0, IconImpl::s_iconsList[ _id ].uv1, _framePadding, _bgColor, _tintColor );
		ImGui::PopID();
		return result;
	}
}
