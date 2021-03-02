#include "core/fanColor.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Color::Color( float _r, float _g, float _b, float _a ) :
            mColorData( glm::vec4( _r, _g, _b, _a ) )
	{}

	//========================================================================================================
	//========================================================================================================
	Color::Color() : mColorData( 1.f, 1.f, 1.f, 1.f ) {}
	Color::Color( const float _rgb ) : mColorData( _rgb, _rgb, _rgb, _rgb ) {}
	Color::Color( const glm::vec4 _rgba ) : mColorData( _rgba.x, _rgba.y, _rgba.z, _rgba.w ) {}
	Color::Color( const ImVec4 _rgba ) : mColorData( _rgba.x, _rgba.y, _rgba.z, _rgba.w ) {}
	Color::Color( const glm::vec3 _rgb, const float _a ) : mColorData( _rgb.x, _rgb.y, _rgb.z, _a ) {}

	//========================================================================================================
	//========================================================================================================
	const Color Color::sRed( 1.f, 0.f, 0.f, 1.f );
	const Color Color::sGreen( 0.f, 1.f, 0.f, 1.f );
	const Color Color::sBlue( 0.f, 0.f, 1.f, 1.f );
	const Color Color::sWhite( 1.f, 1.f, 1.f, 1.f );
	const Color Color::sBlack( 0.f, 0.f, 0.f, 1.f );
	const Color Color::sGrey( 0.5f, 0.5f, 0.5f, 1.f );
    const Color Color::sDarkGrey( 0.25f, 0.25f, 0.25f, 1.f );
	const Color Color::sYellow( 1.f, 1.f, 0.f, 1.f );
	const Color Color::sMagenta( 1.f, 0.f, 1.f, 1.f );
	const Color Color::sCyan( 0.f, 1.f, 1.f, 1.f );
	const Color Color::sBrown( 0.5f, 0.25f, 0.f, 1.f );
	const Color Color::sPurple( 0.5f, 0.f, 1.f, 1.f );
	const Color Color::sPink( 1.f, 0.5f, 0.75f, 1.f );
	const Color Color::sOrange( 1.f, 0.5f, 0.0f, 1.f );
}