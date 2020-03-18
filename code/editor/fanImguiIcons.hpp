#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace ImGui
{

	enum IconType
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

		// special
		GAMEOBJECT16,
		PREFAB16,
		MESH16,

		// scene special
		PLAY16,
		PAUSE16,
		STEP16,
		STOP16,

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

		// generics	
		SOLAR_SYSTEM16,
		HEART16,
		ENERGY16,
		JOYSTICK16,
		SPACE_SHIP16,
		PLANET16,

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
