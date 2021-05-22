#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "core/resources/fanResourceManager.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "engine/resources/fanSceneResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestResourceManager : public UnitTest<UnitTestResourceManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestResourceManager::TestLoad,       "Load" },
                    { &UnitTestResourceManager::TestGet,        "Get" },
                    { &UnitTestResourceManager::TestGetOrLoad,  "Get or load" },
                    { &UnitTestResourceManager::TestRemove,     "Remove" },
                    { &UnitTestResourceManager::TestCount,      "Count" },
                    { &UnitTestResourceManager::TestResolvePtr, "Resolve Ptr" },
                    { &UnitTestResourceManager::TestClear,      "Clear" },
                    { &UnitTestResourceManager::TestDuplicates, "Duplicates" },
            };
        }

        ResourceManager mResources;
        static constexpr const char* sPrefabName0 = "_default/prefab/unit_test0.prefab";
        static constexpr const char* sPrefabName1 = "_default/prefab/unit_test1.prefab";

        void Create() override
        {

            Resource* (* loadPrefab)( const std::string& _path, ResourceInfo& ) =
            []( const std::string& _path, ResourceInfo& )
            {
                Prefab* prefab = new Prefab();
                if( prefab->CreateFromFile( _path ) )
                {
                    return (Resource*)prefab;
                }
                else
                {
                    delete prefab;
                    return (Resource*)nullptr;
                }
            };

            ResourceInfo info;
            info.mLoad = loadPrefab;
            mResources.AddResourceType<Prefab>( info );
        }

        void Destroy() override
        {
        }

        void TestLoad()
        {
            Prefab* prefab = mResources.Load<Prefab>( "" );
            TEST_ASSERT( prefab == nullptr );

            prefab = mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( prefab != nullptr );
            TEST_ASSERT( prefab->mPath == sPrefabName0 );
        }

        void TestGet()
        {
            mResources.Load<Prefab>( "invalid_path" );
            TEST_ASSERT( mResources.Get<Prefab>( "invalid_path" ) == nullptr );
            Prefab* prefab0 = mResources.Load<Prefab>( sPrefabName0 );
            Prefab* prefab1 = mResources.Load<Prefab>( sPrefabName1 );
            TEST_ASSERT( prefab0 != nullptr );
            TEST_ASSERT( prefab1 != nullptr );
            TEST_ASSERT( mResources.Get<Prefab>( sPrefabName0 ) == prefab0 );
            TEST_ASSERT( mResources.Get<Prefab>( sPrefabName1 ) == prefab1 );

            std::vector<Prefab*> prefabs;
            mResources.Get<Prefab>( prefabs );
            TEST_ASSERT( prefabs.size() == 2 );
        }

        void TestGetOrLoad()
        {
            Prefab* prefab = mResources.GetOrLoad<Prefab>( "" );
            TEST_ASSERT( prefab == nullptr );

            Prefab* prefab0 = mResources.GetOrLoad<Prefab>( sPrefabName0 );
            TEST_ASSERT( prefab0 != nullptr );

            Prefab* prefab1 = mResources.Load<Prefab>( sPrefabName1 );
            TEST_ASSERT( prefab1 != nullptr );
            TEST_ASSERT( mResources.GetOrLoad<Prefab>( sPrefabName1 ) == prefab1 );
        }

        void TestRemove()
        {
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
            bool result = mResources.Remove<Prefab>( sPrefabName0 );;
            TEST_ASSERT( !result );
            mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( mResources.Get<Prefab>( sPrefabName0 ) != nullptr );
            result = mResources.Remove<Prefab>( sPrefabName0 );
            TEST_ASSERT( result );
            result = mResources.Remove<Prefab>( sPrefabName0 );
            TEST_ASSERT( !result );
            TEST_ASSERT( mResources.Get<Prefab>( sPrefabName0 ) == nullptr );
        }

        void TestCount()
        {
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
            mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( mResources.Count<Prefab>() == 1 );
            mResources.Load<Prefab>( sPrefabName1 );
            TEST_ASSERT( mResources.Count<Prefab>() == 2 );
            mResources.Remove<Prefab>( sPrefabName0 );
            TEST_ASSERT( mResources.Count<Prefab>() == 1 );
            mResources.Remove<Prefab>( sPrefabName1 );
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
        }

        void TestResolvePtr()
        {
            Prefab* prefab = mResources.Load<Prefab>( sPrefabName0 );
            ResourcePtr<Prefab> rscPtr;
            TEST_ASSERT( rscPtr.mData.mResource == nullptr );
            rscPtr.mData.mPath = sPrefabName0;
            mResources.ResolvePtr( rscPtr.mData );
            TEST_ASSERT( rscPtr.IsValid() );
            TEST_ASSERT( prefab == rscPtr );
        }

        void TestClear()
        {
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
            mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( mResources.Count<Prefab>() == 1 );
            mResources.Clear();
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
        }

        void TestDuplicates()
        {
            Prefab* prefab0 = mResources.Load<Prefab>( sPrefabName0 );
            Prefab* prefab1 = mResources.Load<Prefab>( sPrefabName0 );

            TEST_ASSERT( prefab0 != nullptr );
            TEST_ASSERT( prefab1 != nullptr );
            TEST_ASSERT( prefab0 != prefab1 );
        }
    };
}