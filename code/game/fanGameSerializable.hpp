#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	class MeshPtr;
	class TexturePtr;
	class PrefabPtr;
	class GameobjectPtr;
	class ComponentPtrBase;

	class Scene;

	namespace Serializable
	{
		void SaveGameobjectPtr( Json& _json, const char* _name, const GameobjectPtr& _ptr );
		void SaveComponentPtr(Json& _json, const char* _name, const ComponentPtrBase& _ptr);
		void SaveTexturePtr( Json& _json, const char* _name, const TexturePtr& _ptr );
		void SaveMeshPtr( Json& _json, const char* _name, const MeshPtr& _ptr );
		void SavePrefabPtr( Json& _json, const char* _name, const PrefabPtr& _ptr );

		bool LoadGameobjectPtr( const Json& _json, Scene& _scene, const char* _name, GameobjectPtr& _outPtr );
		bool LoadComponentPtr(const Json& _json, Scene& _scene, const char* _name, ComponentPtrBase& _outPtr);
		bool LoadTexturePtr( const Json& _json, const char* _name, TexturePtr& _outPtr );
		bool LoadMeshPtr( const Json& _json, const char* _name, MeshPtr& _outPtr );
		bool LoadPrefabPtr( const Json& _json, const char* _name, PrefabPtr& _outPtr );
	}
}