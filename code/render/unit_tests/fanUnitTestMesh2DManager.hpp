#pragma once

#include "core/fanUnitTest.hpp"
#include "render/fanMesh2DManager.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/fanMesh2D.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestMesh2DManager : public UnitTest<UnitTestMesh2DManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestMesh2DManager::TestAddMesh,            "Add" },
                    { &UnitTestMesh2DManager::TestMeshRemove,         "Remove" },
                    { &UnitTestMesh2DManager::TestMeshIndexing,       "Indexing" },
                    { &UnitTestMesh2DManager::TestClear,              "Clear" },
                    { &UnitTestMesh2DManager::TestBuffersCreation,    "Buffers creation" },
                    { &UnitTestMesh2DManager::TestBuffersDestruction, "Buffers destruction" },
            };
        }
        void Create() override {}
        void Destroy() override
        {
            Device * device = nullptr;  // when testing we dont generate gpu buffers
            meshManager.Clear( *device );
        }

        Mesh2DManager meshManager;

        void TestAddMesh()
        {
            TEST_ASSERT( meshManager.Empty() );
            Mesh2D* mesh2D = new Mesh2D;
            meshManager.Add( mesh2D, "toto" );
            TEST_ASSERT( ! meshManager.Empty() );
            TEST_ASSERT( meshManager.MeshCount() == 1 );
        }

        void TestMeshIndexing()
        {
            Mesh2D* mesh1 = new Mesh2D;
            Mesh2D* mesh2 = new Mesh2D;
            Mesh2D* mesh3 = new Mesh2D;

            meshManager.Add( mesh1, "mesh1" );
            meshManager.Add( mesh2, "mesh2" );
            meshManager.Add( mesh3, "mesh3" );

            TEST_ASSERT( mesh1->mIndex == 0 );
            TEST_ASSERT( mesh2->mIndex == 1 );
            TEST_ASSERT( mesh3->mIndex == 2 );

            meshManager.Remove( mesh2->mPath );
            TEST_ASSERT( mesh1->mIndex == 0 );
            TEST_ASSERT( mesh3->mIndex == 1 );
        }

        void TestMeshRemove()
        {
            const std::string meshName;
            Mesh2D* mesh2D = new Mesh2D;
            meshManager.Add( mesh2D, meshName );
            TEST_ASSERT( meshManager.Get( meshName ) == mesh2D );
            meshManager.Remove( meshName );
            TEST_ASSERT( meshManager.Get( meshName ) == nullptr );
            TEST_ASSERT( meshManager.Empty() );
        }

        void TestClear() {
            Mesh2D* mesh1 = new Mesh2D;;
            meshManager.Add( mesh1, "mesh1" );
            Device * device = nullptr;
            meshManager.Clear( *device );
            TEST_ASSERT( meshManager.Empty() );
        }

        void TestBuffersCreation()
        {
            Mesh2D* mesh1 = new Mesh2D;
            Mesh2D* mesh2 = new Mesh2D;

            TEST_ASSERT( ! mesh1->mBuffersOutdated  );
            TEST_ASSERT( ! mesh2->mBuffersOutdated  );

            meshManager.Add( mesh1, "mesh1" );
            meshManager.Add( mesh2, "mesh2" );

            mesh1->LoadFromVertices({});
            mesh2->LoadFromVertices({});

            TEST_ASSERT( mesh1->mBuffersOutdated );
            TEST_ASSERT( mesh2->mBuffersOutdated );

            Device * device = nullptr;
            meshManager.CreateBuffers( *device );

            TEST_ASSERT( ! mesh1->mBuffersOutdated  );
            TEST_ASSERT( ! mesh2->mBuffersOutdated  );
        }

        void TestBuffersDestruction()
        {
            Mesh2D* mesh1 = new Mesh2D;;
            const std::string meshName = "toto";
            meshManager.Add( mesh1, meshName );
            TEST_ASSERT( meshManager.DestroyListSize() == 0 );
            meshManager.Remove( meshName );
            TEST_ASSERT( meshManager.DestroyListSize() == 3 );
            Device * device = nullptr;
            meshManager.Clear( * device );
            TEST_ASSERT( meshManager.DestroyListSize() == 0 );
        }
    };
}