#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/ressources/fanRessourcePtr.hpp"

namespace fan
{
	class Texture;
	class Mesh;

	using TexturePtr = RessourcePtr<Texture, std::string >;
	using MeshPtr = RessourcePtr<Mesh, std::string >;
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanTexture( const char * _label, fan::TexturePtr * _ptr );
	bool FanMesh( const char * _label, fan::MeshPtr * _ptr );
}
