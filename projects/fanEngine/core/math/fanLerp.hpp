#pragma once

namespace fan
{
    //==================================================================================================================================================================================================
    // @wip
    // a collection of easing functions
    //==================================================================================================================================================================================================
    namespace Math
    {
        inline Vector3 Lerp( const Vector3 _begin, const Vector3 _end, float _delta )
        {
            return _begin + _delta * ( _end - _begin );
        }

        inline float Lerp( const float _begin, const float _end, float _delta )
        {
            return _begin + _delta * ( _end - _begin );
        }

//		static float EaseLinear( float _val )   { return _val; }
// 		static float EaseOutExpo2( float _val ) { return _val * _val; }
// 		static float EaseOutExpo3( float _val ) { return _val * _val * _val; }
// 		static float EaseOutExpo4( float _val ) { return _val * _val * _val * _val; }
// 		static float EaseOutExpo5( float _val ) { return _val * _val * _val * _val * _val; }
    };
}