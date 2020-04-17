#include "core/fanColor.hpp"

namespace fan
{

	//================================================================================================================================
	// returns a color corresponding to a rtt time in seconds
	//================================================================================================================================
	static ImVec4 GetRttColor( const float _rtt )
	{
		if( _rtt < 0.1 ) { return Color::Green.ToImGui(); }
		if( _rtt < 0.15 ) { return Color::Yellow.ToImGui(); }
		if( _rtt < 0.2 ) { return Color::Orange.ToImGui(); }
		else { return Color::Red.ToImGui(); }
	}
}