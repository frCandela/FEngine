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
	const float IconImpl::s_sizeIcon = 32.f;

	#define DECL_ICON( _x, _y) {{ (_x) *s_sizeIcon / s_iconImageSize, (_y) * s_sizeIcon / s_iconImageSize},{ (_x+1) * s_sizeIcon /s_iconImageSize,(_y+1) * s_sizeIcon /s_iconImageSize}}

	//================================================================================================================================
	//================================================================================================================================
	const IconImpl::IconData IconImpl::s_iconsList[IconType::NUM_ICONS] = {
		 DECL_ICON ( 0,0 )	// NONE

		 // EDITOR
		 ,DECL_ICON ( 0, 1 ) // RENDERER
		 ,DECL_ICON ( 1, 1 ) // SCENE
		 ,DECL_ICON ( 2, 1 ) // INSPECTOR
		 ,DECL_ICON ( 3, 1 ) // PREFERENCES
		 ,DECL_ICON ( 4, 1 ) // CONSOLE
		 ,DECL_ICON ( 5, 1 ) // ECS

		// COMPONENTS
		, DECL_ICON ( 0, 3 ) // CAMERA
		, DECL_ICON ( 1, 3 ) // BOX_SHAPE
		, DECL_ICON ( 2, 3 ) // SPHERE_SHAPE
		, DECL_ICON ( 3, 3 ) // DIR_LIGHT
		, DECL_ICON ( 4, 3 ) // POINT_LIGHT
		, DECL_ICON ( 5, 3 ) // MATERIAL
		, DECL_ICON ( 6, 3 ) // MODEL
		, DECL_ICON ( 7, 3 ) // RIGIDBODY
		, DECL_ICON ( 8, 3 ) // TRANSFORM

		//GAME
		, DECL_ICON ( 0, 5 ) // PLANET
		, DECL_ICON ( 1, 5 ) // SOLAR_SYSTEM
		, DECL_ICON ( 2, 5 ) // SPACE_SHIP
		, DECL_ICON ( 3, 5 ) // GAME_MANAGER
	};
}