#pragma once

#include "imgui/imgui.h"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // rgba color
    //==================================================================================================================================================================================================
    class Color
    {
    public:
        Color();
        Color( const float _rgb );
        Color( const float _r, const float _g, const float _b, const float _a = 1.f );
        Color( const glm::vec4 _rgba );
        Color( const glm::vec3 _rgb, const float _a );
        Color( const ImVec4 _rgba );

        float* Data() { return &mColorData.x; }

        glm::vec4 ToGLM() const { return mColorData; }
        glm::vec3 ToGLM3() const { return glm::vec3( mColorData.x, mColorData.y, mColorData.z ); }
        ImVec4 ToImGui() const { return ImVec4( mColorData.x, mColorData.y, mColorData.z, mColorData.w ); }

        float& operator[]( const int _index ) { return mColorData[_index]; }
        float operator[]( const int _index ) const { return mColorData[_index]; }

        static const Color sRed;
        static const Color sGreen;
        static const Color sBlue;
        static const Color sWhite;
        static const Color sBlack;
        static const Color sGrey;
        static const Color sDarkGrey;
        static const Color sYellow;
        static const Color sMagenta;
        static const Color sCyan;
        static const Color sBrown;
        static const Color sPurple;
        static const Color sPink;
        static const Color sOrange;

    private:
        glm::vec4 mColorData;
    };
}