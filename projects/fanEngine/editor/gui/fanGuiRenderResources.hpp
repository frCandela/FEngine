#pragma once

#include "core/resources/fanResourcePtr.hpp"
//#include "render/resources/fanMesh.hpp"
//#include "render/resources/fanTexture.hpp"

namespace fan
{
    struct Texture;
    struct Mesh;
}

//============================================================================================================
// ImGui widgets
//============================================================================================================
namespace ImGui
{
    bool FanTexturePtr( const char* _label, fan::ResourcePtr<fan::Texture>& _ptr );
    bool FanMeshPtr( const char* _label, fan::ResourcePtr<fan::Mesh>& _ptr );
}