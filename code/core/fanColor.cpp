#include "core/fanColor.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Color::Color( float _r, float _g, float _b, float _a ) :
		m_colorData( glm::vec4( _r, _g, _b, _a ) )
	{}

	//================================================================================================================================
	//================================================================================================================================
	Color::Color() : m_colorData( 1.f, 1.f, 1.f, 1.f ) {}
	Color::Color( const float _rgb ) : m_colorData( _rgb, _rgb, _rgb, _rgb ) {}
	Color::Color( const glm::vec4 _rgba ) : m_colorData( _rgba.x, _rgba.y, _rgba.z, _rgba.w ) {}
	Color::Color( const ImVec4 _rgba ) : m_colorData( _rgba.x, _rgba.y, _rgba.z, _rgba.w ) {}
	Color::Color( const glm::vec3 _rgb, const float _a ) : m_colorData( _rgb.x, _rgb.y, _rgb.z, _a ) {}

	//================================================================================================================================
	//================================================================================================================================
	const Color Color::Red( 1.f, 0.f, 0.f, 1.f );
	const Color Color::Green( 0.f, 1.f, 0.f, 1.f );
	const Color Color::Blue( 0.f, 0.f, 1.f, 1.f );
	const Color Color::White( 1.f, 1.f, 1.f, 1.f );
	const Color Color::Black( 0.f, 0.f, 0.f, 1.f );
	const Color Color::Grey( 0.5f, 0.5f, 0.5f, 1.f );
    const Color Color::sDarkGrey( 0.25f, 0.25f, 0.25f, 1.f );
	const Color Color::Yellow( 1.f, 1.f, 0.f, 1.f );
	const Color Color::Magenta( 1.f, 0.f, 1.f, 1.f );
	const Color Color::Cyan( 0.f, 1.f, 1.f, 1.f );
	const Color Color::Brown( 0.5f, 0.25f, 0.f, 1.f );
	const Color Color::Purple( 0.5f, 0.f, 1.f, 1.f );
	const Color Color::Pink( 1.f, 0.5f, 0.75f, 1.f );
	const Color Color::Orange( 1.f, 0.5f, 0.0f, 1.f );
}