#pragma once

#include <map>

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
        PrefabManager(){}
        PrefabManager( PrefabManager const& ) = delete;
        PrefabManager& operator=( PrefabManager const& ) = delete;

		void Clear();

		Prefab* FindPrefab( const std::string& _path );
		Prefab* LoadPrefab( const std::string& _path );

		const std::map< std::string, Prefab* >& GetList() { return m_prefabs; }
		void ResolvePtr( ResourcePtr<Prefab>& _resourcePtr );
	private:

		std::map< std::string, Prefab* > m_prefabs;

		void RegisterPrefab( Prefab* _prefab );

		// Callbacks
	};
}
