#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Texture;
	class Mesh;

	using TexturePtr = ResourcePtr<Texture>;
	using MeshPtr = ResourcePtr<Mesh >;
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanTexture( const char* _label, fan::TexturePtr* _ptr );
	bool FanMesh( const char* _label, fan::MeshPtr* _ptr );
}
