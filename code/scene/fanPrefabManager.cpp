#include "scene/fanPrefabManager.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanPrefab.hpp"
#include "core/fanSignal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void PrefabManager::Init()
	{
		ResourcePtr< Prefab >::s_onResolve.Connect ( &PrefabManager::ResolvePtr, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab* PrefabManager::FindPrefab( const std::string& _path )
	{
		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
		auto it = m_prefabs.find( cleanPath );
		return  it != m_prefabs.end() ? it->second : nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab* PrefabManager::LoadPrefab( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();

		Prefab* prefab = FindPrefab( cleanPath );
		if ( prefab != nullptr )
		{
			return prefab;
		}
		else
		{
			// Load
			prefab = new Prefab();
			if ( prefab->CreateFromFile( cleanPath ) )
			{
				m_prefabs[ cleanPath ] = prefab;
				return prefab;
			}
			delete prefab;
			return nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PrefabManager::RegisterPrefab( Prefab* _prefab )
	{
		const std::string cleanPath = std::filesystem::path( _prefab->GetPath() ).make_preferred().string();
		assert( m_prefabs.find( cleanPath ) == m_prefabs.end() );
		m_prefabs[ cleanPath ] = _prefab;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PrefabManager::ResolvePtr( ResourcePtr<Prefab>& _resourcePtr )
	{
		assert( !_resourcePtr.IsValid() );

		PrefabPtr& prefabPtr = static_cast< PrefabPtr& >( _resourcePtr );
		Prefab * prefab = LoadPrefab( prefabPtr.GetPath() );

		if ( prefab != nullptr )
		{
			prefabPtr = prefab;
		}
	}

	//================================================================================================================================
    // Deletes all cached prefabs
	//================================================================================================================================
	void PrefabManager::Clear()
	{
		for ( auto pair : m_prefabs )
		{
			delete pair.second;
		}
		m_prefabs.clear();
	}
}