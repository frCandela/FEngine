#pragma once

#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct NoiseOctave
    {
        Fixed mAmplitude    = FIXED( 0.8 );
        Fixed mFrequency    = FIXED( 0.06 );
        Fixed mHeightWeight = FIXED( 0.5 );
        Fixed mHeightOffset= FIXED( 0 );
        Fixed mWeight= FIXED( 1 );

        static void Save( Json& _json, const char* _name, const NoiseOctave& _octave )
        {
            Serializable::SaveFixed( _json[_name], "amplitude", _octave.mAmplitude );
            Serializable::SaveFixed( _json[_name], "frequency", _octave.mFrequency );
            Serializable::SaveFixed( _json[_name], "height_weight", _octave.mHeightWeight );
            Serializable::SaveFixed( _json[_name], "height_offset", _octave.mHeightOffset );
            Serializable::SaveFixed( _json[_name], "weight", _octave.mWeight );
        }
        static void Load( const Json& _json, const char* _name, NoiseOctave& _octave )
        {
            Serializable::LoadFixed( _json[_name], "amplitude", _octave.mAmplitude );
            Serializable::LoadFixed( _json[_name], "frequency", _octave.mFrequency );
            Serializable::LoadFixed( _json[_name], "height_weight", _octave.mHeightWeight );
            Serializable::LoadFixed( _json[_name], "height_offset", _octave.mHeightOffset );
            Serializable::LoadFixed( _json[_name], "weight", _octave.mWeight );
        }
    };
}