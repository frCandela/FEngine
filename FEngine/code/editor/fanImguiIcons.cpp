#include "fanGlobalIncludes.h"
#include "editor/fanImguiIcons.h"

namespace ImGui {

	//================================================================================================================================
	//================================================================================================================================
	struct IconData {
		ImVec2 uv0;
		ImVec2 uv1;
	};

	const float IconImpl::s_iconImageSize = 512.f;

	#define DECL_ICON(_size, _x, _y) {{ (_x) * _size / s_iconImageSize, (_y) * _size / s_iconImageSize},{ (_x+1) * _size /s_iconImageSize,(_y+1) * _size /s_iconImageSize}}

	//================================================================================================================================
	//================================================================================================================================
	const IconImpl::IconData IconImpl::s_iconsList[IconType::NUM_ICONS] = {
		 DECL_ICON (32.f, 0,0 )	// NONE

		 // EDITOR
		 ,DECL_ICON ( 32.f, 0, 1 ) // RENDERER
		 ,DECL_ICON ( 32.f, 1, 1 ) // IMAGE
		 ,DECL_ICON ( 32.f, 2, 1 ) // INSPECTOR
		 ,DECL_ICON ( 32.f, 3, 1 ) // PREFERENCES
		 ,DECL_ICON ( 32.f, 4, 1 ) // CONSOLE
		 ,DECL_ICON ( 32.f, 5, 1 ) // ECS
		 ,DECL_ICON ( 32.f, 6, 1 ) // IMGUI
		 ,DECL_ICON ( 32.f, 7, 1 ) // PROFILER
		 ,DECL_ICON ( 32.f, 8, 1 ) // SCENE
		 ,DECL_ICON ( 32.f, 9, 1 ) // GAMEOBJECT
		 ,DECL_ICON ( 16.f, 0, 4 ) // GAMEOBJECT16
		
		// COMPONENTS
		, DECL_ICON ( 32.f, 0, 3 ) // CAMERA
		, DECL_ICON ( 32.f, 1, 3 ) // BOX_SHAPE
		, DECL_ICON ( 32.f, 2, 3 ) // SPHERE_SHAPE
		, DECL_ICON ( 32.f, 3, 3 ) // DIR_LIGHT
		, DECL_ICON ( 32.f, 4, 3 ) // POINT_LIGHT
		, DECL_ICON ( 32.f, 5, 3 ) // MATERIAL
		, DECL_ICON ( 32.f, 6, 3 ) // MESH
		, DECL_ICON ( 32.f, 7, 3 ) // RIGIDBODY
		, DECL_ICON ( 32.f, 8, 3 ) // TRANSFORM
		, DECL_ICON ( 32.f, 9, 3 ) // PARTICLES		
						  
		//GAME		  	  
		, DECL_ICON ( 32.f, 0, 5 ) // PLANET
		, DECL_ICON ( 32.f, 1, 5 ) // SOLAR_SYSTEM
		, DECL_ICON ( 32.f, 2, 5 ) // SPACE_SHIP
		, DECL_ICON ( 32.f, 3, 5 ) // GAME_MANAGER

		// UI
		, DECL_ICON ( 32.f, 0, 7 ) // PROGRESS_BAR
		
	};
}