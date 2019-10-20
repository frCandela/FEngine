#pragma once

namespace ImGui {

	enum IconType { 
		NONE = 0,
		RENDERER, SCENE, INSPECTOR, PREFERENCES, CONSOLE, ECS, IMGUI,
		CAMERA, BOX_SHAPE, SPHERE_SHAPE, DIR_LIGHT, POINT_LIGHT, MATERIAL,MODEL,RIGIDBODY,TRANSFORM,
		PLANET, SOLAR_SYSTEM, SPACE_SHIP, GAME_MANAGER,
		NUM_ICONS
	};

	//================================================================================================================================
	//================================================================================================================================
	struct IconImpl {
		struct IconData {
			ImVec2 uv0;
			ImVec2 uv1;
		};
		static const float s_iconImageSize;
		static const float s_sizeIcon;
		static const IconData s_iconsList[];
	};

	//================================================================================================================================
	//================================================================================================================================
	inline void Icon(	const IconType _id, 
						const ImVec2& _size, 
						const ImVec4& _tintColor = ImVec4( 1, 1, 1, 1 ), 
						const ImVec4& _borderColor = ImVec4( 0, 0, 0, 0 ) ) 
	{
		ImGui::Image( (void*)42, _size, IconImpl::s_iconsList[_id].uv0, IconImpl::s_iconsList[_id].uv1, _tintColor, _borderColor );
	}
}
