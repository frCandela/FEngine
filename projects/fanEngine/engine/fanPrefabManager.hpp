#pragma once

#include <map>

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
    class Prefab;

    //==================================================================================================================================================================================================
    // Loads & references all the prefabs of the engine
    //==================================================================================================================================================================================================
    class PrefabManager
    {
    public:
        PrefabManager() {}
        ~PrefabManager();
        PrefabManager( PrefabManager const& ) = delete;
        PrefabManager& operator=( PrefabManager const& ) = delete;

        Prefab* Get( const std::string& _path );
        Prefab* Load( const std::string& _path );
        void Remove( const std::string& _path );
        void Clear();
        bool Empty() const { return mPrefabs.empty(); }
        void ResolvePtr( ResourcePtr <Prefab>& _resourcePtr );

        const std::map<std::string, Prefab*>& GetPrefabs() const { return mPrefabs; }

    private:
        std::map<std::string, Prefab*> mPrefabs;
    };
}
