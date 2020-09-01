#include "scene/fanPrefabManager.hpp"

#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    PrefabManager::~PrefabManager(){ Clear(); }

	//========================================================================================================
	//========================================================================================================
	Prefab* PrefabManager::Get( const std::string& _path )
	{
		auto it = mPrefabs.find( _path );
		return it != mPrefabs.end() ? it->second : nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	Prefab* PrefabManager::Load( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		Prefab* prefab = Get( _path );
		if ( prefab != nullptr )
		{
			return prefab;
		}
		else
		{
			// Load
			prefab = new Prefab();
			if ( prefab->CreateFromFile( _path ) )
			{
                mPrefabs[ _path ] = prefab;
				return prefab;
			}
			delete prefab;
			return nullptr;
		}
	}

    //========================================================================================================
    //========================================================================================================
    void PrefabManager::Remove( const std::string& _path )
    {
	    auto it = mPrefabs.find( _path );
	    if( it != mPrefabs.end() )
        {
            delete it->second;
            mPrefabs.erase( it );
        }
    }

	//========================================================================================================
	//========================================================================================================
	void PrefabManager::ResolvePtr( ResourcePtr<Prefab>& _resourcePtr )
	{
        fanAssert( !_resourcePtr.IsValid() );

		PrefabPtr& prefabPtr = static_cast< PrefabPtr& >( _resourcePtr );
		Prefab * prefab = Load( prefabPtr.GetPath() );

		if ( prefab != nullptr )
		{
			prefabPtr.Set( prefab );
		}
	}

	//========================================================================================================
    // Deletes all cached prefabs
	//========================================================================================================
	void PrefabManager::Clear()
	{
		for ( auto pair : mPrefabs )
		{
			delete pair.second;
		}
		mPrefabs.clear();
	}
}