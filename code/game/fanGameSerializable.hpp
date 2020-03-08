#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	class PrefabPtr;
	class GameobjectPtr;
	class ComponentPtrBase;

	class Scene;

	namespace Serializable
	{
		void SaveGameobjectPtr( Json& _json, const char* _name, const GameobjectPtr& _ptr );
		void SaveComponentPtr(Json& _json, const char* _name, const ComponentPtrBase& _ptr);
		void SavePrefabPtr( Json& _json, const char* _name, const PrefabPtr& _ptr );

		bool LoadGameobjectPtr( const Json& _json, Scene& _scene, const char* _name, GameobjectPtr& _outPtr );
		bool LoadComponentPtr(const Json& _json, Scene& _scene, const char* _name, ComponentPtrBase& _outPtr);
		bool LoadPrefabPtr( const Json& _json, const char* _name, PrefabPtr& _outPtr );
	}
}