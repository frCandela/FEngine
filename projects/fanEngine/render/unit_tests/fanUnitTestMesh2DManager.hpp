#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/resources/fanMesh2D.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestMesh2DManager : public UnitTest<UnitTestMesh2DManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestMesh2DManager::TestAddMesh,      "Add" },
                    { &UnitTestMesh2DManager::TestMeshRemove,   "Remove" },
                    { &UnitTestMesh2DManager::TestMeshIndexing, "Indexing" },
                    { &UnitTestMesh2DManager::TestClear,        "Clear" },
                    { &UnitTestMesh2DManager::TestCreate,       "Create" },
                    { &UnitTestMesh2DManager::TestDestroy,      "Destroy" },
            };
        }
        void Create() override {}
        void Destroy() override
        {
            Device* device = nullptr;  // when testing we dont generate gpu buffers
            mMeshManager.Clear( *device );
        }

        Mesh2DManager mMeshManager;

        void TestAddMesh()
        {
            TEST_ASSERT( mMeshManager.Empty() );
            Mesh2D* mesh2D = new Mesh2D;
            const std::string meshName = "toto";
            mMeshManager.Add( mesh2D, meshName );
            TEST_ASSERT( mesh2D->mPath == meshName );
            TEST_ASSERT( !mMeshManager.Empty() );
            TEST_ASSERT( mMeshManager.MeshCount() == 1 );
        }

        void TestMeshIndexing()
        {
            Mesh2D* mesh1 = new Mesh2D;
            Mesh2D* mesh2 = new Mesh2D;
            Mesh2D* mesh3 = new Mesh2D;

            mMeshManager.Add( mesh1, "mesh1" );
            mMeshManager.Add( mesh2, "mesh2" );
            mMeshManager.Add( mesh3, "mesh3" );

            TEST_ASSERT( mesh1->mIndex == 0 );
            TEST_ASSERT( mesh2->mIndex == 1 );
            TEST_ASSERT( mesh3->mIndex == 2 );

            mMeshManager.Remove( mesh2->mPath );
            TEST_ASSERT( mesh1->mIndex == 0 );
            TEST_ASSERT( mesh3->mIndex == 1 );
        }

        void TestMeshRemove()
        {
            const std::string meshName;
            Mesh2D* mesh2D = new Mesh2D;
            mMeshManager.Add( mesh2D, meshName );
            TEST_ASSERT( mMeshManager.Get( meshName ) == mesh2D );
            mMeshManager.Remove( meshName );
            TEST_ASSERT( mMeshManager.Get( meshName ) == nullptr );
            TEST_ASSERT( mMeshManager.Empty() );
        }

        void TestClear()
        {
            Mesh2D* mesh1 = new Mesh2D;;
            mMeshManager.Add( mesh1, "mesh1" );
            TEST_ASSERT( !mMeshManager.Empty() );
            Device* device = nullptr;
            mMeshManager.Clear( *device );
            TEST_ASSERT( mMeshManager.Empty() );
        }

        void TestCreate()
        {
            Mesh2D* mesh1 = new Mesh2D;

            TEST_ASSERT( !mesh1->mBuffersOutdated );

            mMeshManager.Add( mesh1, "mesh1" );

            mesh1->LoadFromVertices( {} );

            TEST_ASSERT( mesh1->mBuffersOutdated );

            Device* device = nullptr;
            mMeshManager.CreateNewMeshes( *device );

            TEST_ASSERT( !mesh1->mBuffersOutdated );
        }

        void TestDestroy()
        {
            Mesh2D* mesh1 = new Mesh2D;;
            const std::string meshName = "toto";
            mMeshManager.Add( mesh1, meshName );
            TEST_ASSERT( mMeshManager.DestroyListSize() == 0 );
            mMeshManager.Remove( meshName );
            TEST_ASSERT( mMeshManager.DestroyListSize() == 1 );
            Device* device = nullptr;
            mMeshManager.Clear( *device );
            TEST_ASSERT( mMeshManager.DestroyListSize() == 0 );
        }
    };
}