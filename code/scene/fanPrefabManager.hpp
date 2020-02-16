#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Prefab;

	//================================================================================================================================
	// Loads & references all the prefabs of the engine
	//================================================================================================================================
	class PrefabManager
	{
	public:
		void Init();
		void Clear();

		Prefab* FindPrefab( const std::string& _path );
		Prefab* LoadPrefab( const std::string& _path );

		const std::map< std::string, Prefab* >& GetList() { return m_prefabs; }
	private:
		std::map< std::string, Prefab* > m_prefabs;

		void RegisterPrefab( Prefab* _prefab );

		std::string CleanPath( const std::string& _path );

		// Callbacks
		void ResolvePtr( ResourcePtr<Prefab>& _resourcePtr );
	};
}
