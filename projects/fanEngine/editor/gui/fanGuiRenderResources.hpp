#pragma once

namespace fan
{
    class TexturePtr;
    class MeshPtr;
}

//============================================================================================================
// ImGui widgets
//============================================================================================================
namespace ImGui
{
    bool FanTexturePtr( const char* _label, fan::TexturePtr& _ptr );
    bool FanMeshPtr( const char* _label, fan::MeshPtr& _ptr );
}