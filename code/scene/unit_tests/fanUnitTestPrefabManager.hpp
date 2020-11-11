#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestPrefabManager : public UnitTest<UnitTestPrefabManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestPrefabManager::TestLoad,       "Load" },
                     { &UnitTestPrefabManager::TestGet,        "Get" },
                     { &UnitTestPrefabManager::TestRemove,     "Remove" },
                     { &UnitTestPrefabManager::TestClear,      "Clear" },
                     { &UnitTestPrefabManager::TestResolvePtr, "Resolve ptr" },
            };
        }
        void Create() override {}
        void Destroy() override {}

        PrefabManager mPrefabManager;
        static constexpr char * sPrefabName0 = "content/_default/prefab/unit_test0.prefab";
        static constexpr char * sPrefabName1 = "content/_default/prefab/unit_test1.prefab";

        void TestLoad()
        {
            TEST_ASSERT( mPrefabManager.Empty() );
            mPrefabManager.Load( "invalid_path" );
            TEST_ASSERT( mPrefabManager.Empty() );
            Prefab * prefab =  mPrefabManager.Load( sPrefabName0 );
            TEST_ASSERT( prefab != nullptr );
            TEST_ASSERT( prefab->mPath == sPrefabName0 );
            TEST_ASSERT( ! mPrefabManager.Empty() );
        }

        void TestGet()
        {
            mPrefabManager.Load( "invalid_path" );
            TEST_ASSERT( mPrefabManager.Get( "invalid_path" ) == nullptr );
            Prefab * prefab0 = mPrefabManager.Load( sPrefabName0 );
            Prefab * prefab1 = mPrefabManager.Load( sPrefabName1 );
            TEST_ASSERT( prefab0 != nullptr );
            TEST_ASSERT( prefab1 != nullptr );
            TEST_ASSERT( mPrefabManager.Get( sPrefabName0 ) == prefab0 );
            TEST_ASSERT( mPrefabManager.Get( sPrefabName1 ) == prefab1 );
        }

        void TestRemove()
        {
            mPrefabManager.Load( sPrefabName0 );
            TEST_ASSERT( mPrefabManager.Get( sPrefabName0 ) != nullptr );
            mPrefabManager.Remove( sPrefabName0 );
            TEST_ASSERT( mPrefabManager.Get( sPrefabName0 ) == nullptr );
            TEST_ASSERT( mPrefabManager.Empty() );
        }

        void TestClear()
        {
            mPrefabManager.Load( sPrefabName0 );
            TEST_ASSERT( ! mPrefabManager.Empty() );
            mPrefabManager.Clear();
            TEST_ASSERT( mPrefabManager.Empty() );
        }

        void TestResolvePtr()
        {
            Prefab * prefab = mPrefabManager.Load( sPrefabName0 );
            PrefabPtr rscPtr;
            TEST_ASSERT( ! rscPtr.IsValid() );
            rscPtr.Init( sPrefabName0 );
            mPrefabManager.ResolvePtr( rscPtr );
            TEST_ASSERT( rscPtr.IsValid() );
            TEST_ASSERT( prefab == rscPtr.GetResource() );
        }
    };
}