#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "core/fanISerializable.hpp"

namespace fan {
	class Component;
	class Gameobject;
	class Texture;
	class Mesh;
	class Prefab;
	struct IDPtrData;
	template< typename _RessourceType, typename _IDType > class RessourcePtr;
	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t>;
	using TexturePtr = RessourcePtr<Texture, std::string>;
	using MeshPtr = RessourcePtr<Mesh, std::string>;
	using PrefabPtr = RessourcePtr<Prefab, std::string>;
	using ComponentIDPtr = RessourcePtr< Component, IDPtrData >;

	namespace Serializable {
		void SaveGameobjectPtr(Json& _json, const char* _name, const GameobjectPtr& _ptr);
		void SaveTexturePtr(Json& _json, const char* _name, const TexturePtr& _ptr);
		void SaveMeshPtr(Json& _json, const char* _name, const MeshPtr& _ptr);
		void SavePrefabPtr(Json& _json, const char* _name, const PrefabPtr& _ptr);
		void SaveComponentPtr(Json& _json, const char* _name, const ComponentIDPtr& _ptr);

		bool LoadGameobjectPtr(const Json& _json, const char* _name, GameobjectPtr& _outPtr);
		bool LoadTexturePtr(const Json& _json, const char* _name, TexturePtr& _outPtr);
		bool LoadMeshPtr(const Json& _json, const char* _name, MeshPtr& _outPtr);
		bool LoadPrefabPtr(const Json& _json, const char* _name, PrefabPtr& _outPtr);
		bool LoadComponentPtr(const Json& _json, const char* _name, ComponentIDPtr& _outPtr);
	}
}