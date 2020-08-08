#pragma once

#include "imgui/imgui.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

namespace fan
{
	//================================================================================================================================
	// rgba color
	//================================================================================================================================
	class Color
	{
	public:
		Color();
		Color( const float _rgb );
		Color( const float _r, const float _g, const float _b, const float _a = 1.f );
		Color( const glm::vec4 _rgba );
		Color( const glm::vec3 _rgb, const float _a );
		Color( const ImVec4 _rgba );

		float* Data() { return &m_colorData.x; }

		glm::vec4 ToGLM() const { return m_colorData; }
		glm::vec3 ToGLM3() const { return glm::vec3( m_colorData.x, m_colorData.y, m_colorData.z ); }
		ImVec4	  ToImGui() const { return ImVec4( m_colorData.x, m_colorData.y, m_colorData.z, m_colorData.w ); }

		float& operator[]	( const int _index ) { return m_colorData[ _index ]; }
		float operator[]	( const int _index ) const { return m_colorData[ _index ]; }

		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color White;
		static const Color Black;
		static const Color Grey;
        static const Color sDarkGrey;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Cyan;
		static const Color Brown;
		static const Color Purple;
		static const Color Pink;
		static const Color Orange;
	private:
		glm::vec4 m_colorData;
	};
}