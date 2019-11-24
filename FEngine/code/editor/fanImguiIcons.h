#pragma once

namespace ImGui {

	enum IconType { 
		NONE = 0,
		RENDERER, 
		IMAGE, 
		INSPECTOR, 
		PREFERENCES, 
		CONSOLE, 
		ECS, 
		IMGUI, 
		PROFILER, 
		SCENE, 
		GAMEOBJECT, 
		GAMEOBJECT16, 
		PREFAB32, 
		PREFAB16,
		CAMERA, 
		BOX_SHAPE, 
		SPHERE_SHAPE, 
		DIR_LIGHT, 
		POINT_LIGHT, 
		MATERIAL,
		MESH,
		RIGIDBODY,
		TRANSFORM, 
		PARTICLES,
		PLANET, 
		SOLAR_SYSTEM, 
		SPACE_SHIP, 
		GAME_MANAGER, 
		HEART16, 
		ENERGY16,
		PROGRESS_BAR,
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

	//================================================================================================================================
	//================================================================================================================================
	inline bool ButtonIcon( const IconType _id,
		const ImVec2& _size,
		const int _framePadding = -1,
		const ImVec4& _bgColor = ImVec4( 0, 0, 0, 0 ),
		const ImVec4& _tintColor = ImVec4( 1, 1, 1, 1 ) )
	{
		return ImGui::ImageButton( (void*)42, _size, IconImpl::s_iconsList[_id].uv0, IconImpl::s_iconsList[_id].uv1, _framePadding, _bgColor, _tintColor );
	}
}
