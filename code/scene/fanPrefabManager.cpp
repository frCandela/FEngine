#include "scene/fanPrefabManager.hpp"
#include "scene/fanPrefab.hpp"
#include "render/fanRenderRessourcePtr.hpp"
#include "core/fanSignal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void PrefabManager::Init()
	{
		PrefabPtr::s_onCreateUnresolved.Connect ( &PrefabManager::OnResolvePrefabPtr, this);
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab* PrefabManager::FindPrefab(const std::string& _path)
	{
		std::string path = CleanPath(_path);
		auto it = m_prefabs.find(path);
		return  it != m_prefabs.end() ? it->second : nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab* PrefabManager::LoadPrefab(const std::string& _path)
	{
		if (_path.empty()) { return nullptr; }

			// Load
			Prefab* prefab = new Prefab();
		if (prefab->LoadFromFile(CleanPath(_path)))
		{
			RegisterPrefab(prefab);
			return prefab;
		}
		delete prefab;
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PrefabManager::RegisterPrefab(Prefab* _prefab)
	{
		std::string path = CleanPath( _prefab->GetPath() );
		assert( m_prefabs.find(path) == m_prefabs.end() );
		m_prefabs[ path ] = _prefab;
	}

	//================================================================================================================================
	// the / is dead, long live the \ 
	// @todo refacto this with other CleanPath methods (ressourcemanager)
	//================================================================================================================================
	std::string PrefabManager::CleanPath(const std::string& _path)
	{
		std::filesystem::path path = _path;
		path.make_preferred();

		return path.string();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PrefabManager::OnResolvePrefabPtr(PrefabPtr* _ptr)
	{
		Prefab * prefab = FindPrefab( _ptr->GetID() );
		if ( prefab == nullptr )
		{
			prefab = LoadPrefab( _ptr->GetID() );
		}
		if ( prefab )
		{
			*_ptr = PrefabPtr( prefab, prefab->GetPath() );
		}
	}
}