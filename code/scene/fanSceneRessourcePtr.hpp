#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/ressources/fanRessourcePtr.hpp"

namespace fan
{
	class Gameobject;
	class Prefab;

	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t>;
	using PrefabPtr =  RessourcePtr<Prefab, std::string >;
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanGameobject( const char * _label, fan::GameobjectPtr * _ptr );
	bool FanPrefab( const char * _label,  fan::PrefabPtr * _ptr );
}
