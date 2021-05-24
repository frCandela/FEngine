#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "core/resources/fanResources.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "engine/resources/fanSceneResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestResources : public UnitTest<UnitTestResources>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestResources::TestLoad,        "Load" },
                    { &UnitTestResources::TestGet,         "Get" },
                    { &UnitTestResources::TestGetOrLoad,   "Get or load" },
                    { &UnitTestResources::TestRemove,      "Remove" },
                    { &UnitTestResources::TestCount,       "Count" },
                    { &UnitTestResources::TestClear,       "Clear" },
                    { &UnitTestResources::TestDuplicates,  "Duplicates" },
                    { &UnitTestResources::TestDestroyList, "Destroy list" },
                    { &UnitTestResources::TestDirty,       "Dirty" },
            };
        }

        Resources mResources;
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
            info.mLoad           = loadPrefab;
            info.mUseDestroyList = true;
            info.mUseDirtyList = true;
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

            std::vector<ResourcePtr<Prefab>> prefabs;
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
            bool result = mResources.Remove( sPrefabName0 );;
            TEST_ASSERT( !result );
            mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( mResources.Get<Prefab>( sPrefabName0 ) != nullptr );
            result = mResources.Remove( sPrefabName0 );
            TEST_ASSERT( result );
            result = mResources.Remove( sPrefabName0 );
            TEST_ASSERT( !result );
            ResourcePtr<Prefab> ptr = mResources.Get<Prefab>( sPrefabName0 );
            TEST_ASSERT(  ptr == nullptr );
            TEST_ASSERT(  ptr.mData.mHandle != nullptr );
            TEST_ASSERT(  ptr.mData.mGUID != 0 );
        }

        void TestCount()
        {
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
            mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( mResources.Count<Prefab>() == 1 );
            mResources.Load<Prefab>( sPrefabName1 );
            TEST_ASSERT( mResources.Count<Prefab>() == 2 );
            mResources.Remove( sPrefabName0 );
            TEST_ASSERT( mResources.Count<Prefab>() == 1 );
            mResources.Remove( sPrefabName1 );
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
        }

        void TestClear()
        {
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
            ResourcePtr<Prefab> ptr = mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT(  ptr.mData.mHandle != nullptr );
            TEST_ASSERT(  ptr.mData.mHandle->mResource != nullptr );
            TEST_ASSERT( mResources.Count<Prefab>() == 1 );
            mResources.Clear();
            TEST_ASSERT( mResources.Count<Prefab>() == 0 );
            TEST_ASSERT(  ptr.mData.mHandle != nullptr );
            TEST_ASSERT(  ptr.mData.mHandle->mResource == nullptr );
        }

        void TestDuplicates()
        {
            Prefab* prefab0 = mResources.Load<Prefab>( sPrefabName0 );
            Prefab* prefab1 = mResources.Load<Prefab>( sPrefabName0 );

            TEST_ASSERT( prefab0 != nullptr );
            TEST_ASSERT( prefab1 != nullptr );
            TEST_ASSERT( prefab0 != prefab1 );
        }

        void TestDestroyList()
        {
            Prefab* prefab0 = mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( prefab0 != nullptr );

            std::vector<Prefab*> destroyList;
            mResources.GetDestroyList<Prefab>( destroyList );
            TEST_ASSERT( destroyList.size() == 0 );
            mResources.Clear();
            mResources.GetDestroyList<Prefab>( destroyList );
            TEST_ASSERT( destroyList.size() == 1 );
            TEST_ASSERT( destroyList[0] == prefab0 );
            mResources.GetDestroyList<Prefab>( destroyList );
            TEST_ASSERT( destroyList.size() == 0 )

            prefab0 = mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( prefab0 != nullptr );
            mResources.GetDestroyList<Prefab>( destroyList );
            TEST_ASSERT( destroyList.size() == 0 )
            mResources.Remove( prefab0->mGUID );
            mResources.GetDestroyList<Prefab>( destroyList );
            TEST_ASSERT( destroyList.size() == 1 )
            mResources.GetDestroyList<Prefab>( destroyList );
            TEST_ASSERT( destroyList.size() == 0 )
        }

        void TestDirty()
        {
            std::vector<Prefab*> dirtyList;
            mResources.GetDirtyList<Prefab>( dirtyList );
            TEST_ASSERT( dirtyList.size() == 0 );

            Prefab* prefab0 = mResources.Load<Prefab>( sPrefabName0 );
            TEST_ASSERT( prefab0 != nullptr );
            mResources.GetDirtyList<Prefab>( dirtyList );
            TEST_ASSERT( dirtyList.size() == 1 );
            mResources.GetDirtyList<Prefab>( dirtyList );
            TEST_ASSERT( dirtyList.size() == 0 );

            mResources.SetDirty( prefab0->mGUID );
            mResources.GetDirtyList<Prefab>( dirtyList );
            TEST_ASSERT( dirtyList.size() == 1 );
            TEST_ASSERT( dirtyList[0] == prefab0 );
            mResources.GetDirtyList<Prefab>( dirtyList );
            TEST_ASSERT( dirtyList.size() == 0 )
        }
    };
}