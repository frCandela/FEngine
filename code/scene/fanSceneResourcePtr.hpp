#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Gameobject;
	class Prefab;

	using GameobjectPtr = ResourcePtr<Gameobject>;
	using PrefabPtr =  ResourcePtr<Prefab>;
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanGameobject( const char * _label, fan::GameobjectPtr * _ptr );
	bool FanPrefab( const char * _label,  fan::PrefabPtr * _ptr );
}
