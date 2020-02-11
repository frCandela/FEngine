#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "core/fanSingleton.hpp"

namespace fan
{
	class Prefab;
	template< typename _ResourceType > class ResourcePtr;
	using PrefabPtr = ResourcePtr<Prefab>;

	//================================================================================================================================
	// Loads & references all the prefabs of the engine
	//================================================================================================================================
	class PrefabManager : public Singleton<PrefabManager> {
	public:
		friend class Singleton<PrefabManager>;

		void Init();

		Prefab* FindPrefab(const std::string& _path);
		Prefab* LoadPrefab(const std::string& _path);

	private:
		std::map< std::string, Prefab* > m_prefabs;

		void RegisterPrefab(Prefab* _prefab);

		std::string CleanPath(const std::string& _path);

		// Callbacks
		void OnResolvePrefabPtr(PrefabPtr* _ptr);
	};
}
