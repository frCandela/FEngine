#include "editor/fanImguiIcons.hpp"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	struct IconData
	{
		ImVec2 uv0;
		ImVec2 uv1;
	};

	const float IconImpl::s_iconImageSize = 512.f;

#define DECL_ICON(_size, _x, _y) {{ (_x) * _size / s_iconImageSize, ((_y) * _size) / s_iconImageSize},{ (_x+1) * _size /s_iconImageSize,((_y+1) * _size) /s_iconImageSize}}

	//================================================================================================================================
	//================================================================================================================================
	const IconImpl::IconData IconImpl::s_iconsList[ IconType::NUM_ICONS ] = {
		 DECL_ICON( 16.f, 0,0 )	// NONE

		// editor windows
		,DECL_ICON( 16.f, 1, 4 )  // IMGUI16,
		,DECL_ICON( 16.f, 2, 4 )  // RENDERER16,
		,DECL_ICON( 16.f, 3, 4 )  // SCENE16,
		,DECL_ICON( 16.f, 4, 4 )  // INSPECTOR16,
		,DECL_ICON( 16.f, 5, 4 )  // CONSOLE16,
		,DECL_ICON( 16.f, 6, 4 )  // ECS16,
		,DECL_ICON( 16.f, 7, 4 )  // PROFILER16,
		,DECL_ICON( 16.f, 8, 4 )  // PREFERENCES16,
		,DECL_ICON( 16.f, 9, 4 )  // SINGLETON16,

		// scene special
		,DECL_ICON( 16.f, 0, 6 )  // GAMEOBJECT16,
		,DECL_ICON( 16.f, 1, 6 )  // PREFAB16,
		,DECL_ICON( 16.f, 2, 6 )  // MESH16,
		,DECL_ICON( 16.f, 3, 6 )  // LOCK_OPEN16,
		,DECL_ICON( 16.f, 4, 6 )  // LOCK_CLOSED16,
		,DECL_ICON( 16.f, 5, 6 )  // NETWORK16,

		,DECL_ICON( 16.f, 1, 0 )  // PLAY16
		,DECL_ICON( 16.f, 2, 0 )  // PAUSE16
		,DECL_ICON( 16.f, 3, 0 )  // STEP16
		,DECL_ICON( 16.f, 4, 0 )  // STOP16

		// scene components
		,DECL_ICON( 16.f, 0, 8 )  // IMAGE16,
		,DECL_ICON( 16.f, 1, 8 )  // CAMERA16,
		,DECL_ICON( 16.f, 2, 8 )  // TRANSFORM16,
		,DECL_ICON( 16.f, 3, 8 )  // PARTICLES16,
		,DECL_ICON( 16.f, 4, 8 )  // DIR_LIGHT16,
		,DECL_ICON( 16.f, 5, 8 )  // POINT_LIGHT16,
		,DECL_ICON( 16.f, 6, 8 )  // MATERIAL16,
		,DECL_ICON( 16.f, 7, 8 )  // MESH_RENDERER16,
		,DECL_ICON( 16.f, 8, 8 )  // RIGIDBODY16,
		,DECL_ICON( 16.f, 9, 8 )  // CUBE_SHAPE16,
		,DECL_ICON( 16.f, 10, 8 ) // SPHERE_SHAPE16,
		,DECL_ICON( 16.f, 11, 8 ) // UI_MESH_RENDERER16,
		,DECL_ICON( 16.f, 12, 8 ) // UI_TRANSFORM16,
		,DECL_ICON( 16.f, 13, 8 ) // UI_PROGRESS_BAR16,
		,DECL_ICON( 16.f, 14, 8 ) // FOLLOW_TRANSFORM
		,DECL_ICON( 16.f, 15, 8 ) // INPUT16
		,DECL_ICON( 16.f, 16, 8 ) // FOLLOW_TRANSFORM_UI16
		,DECL_ICON( 16.f, 17, 8 ) // EXPIRATION16

		,DECL_ICON( 16.f, 0, 2 ) // TIME16
		,DECL_ICON( 16.f, 1, 2 ) // SPAWN16
		,DECL_ICON( 16.f, 2, 2 ) // CONNECTION16
		,DECL_ICON( 16.f, 3, 2 ) // SOCKET16
		,DECL_ICON( 16.f, 4, 2 ) // PTR16
		,DECL_ICON( 16.f, 5, 2 ) // GRID16
		,DECL_ICON( 16.f, 6, 2 ) // SELECTION16
		,DECL_ICON( 16.f, 7, 2 ) // GIZMOS16
		,DECL_ICON( 16.f, 8, 2 ) // COPY_PASTE16
		,DECL_ICON( 16.f, 9, 2 ) // LINK16
		,DECL_ICON( 16.f, 10, 2 ) // CLIENT_NET16
		,DECL_ICON( 16.f, 11, 2 ) // RENDER_DEBUG16

		// generics	
		,DECL_ICON( 16.f, 0, 10 )  // SOLAR_SYSTEM16,
		,DECL_ICON( 16.f, 1, 10 )  // HEART16,
		,DECL_ICON( 16.f, 2, 10 )  // ENERGY16,
		,DECL_ICON( 16.f, 3, 10 )  // JOYSTICK16,
		,DECL_ICON( 16.f, 4, 10 )  // SPACE_SHIP16,
		,DECL_ICON( 16.f, 5, 10 )  // PLANET16,		
		,DECL_ICON( 16.f, 6, 10 )  // SUN16,	
		,DECL_ICON( 16.f, 7, 10 )  // SUN_RED16,	

		//network
		,DECL_ICON( 16.f, 0, 12 )  // RELIABILITY_LAYER16,	
		,DECL_ICON( 16.f, 1, 12 )  // GAME_DATA16,
	};

}