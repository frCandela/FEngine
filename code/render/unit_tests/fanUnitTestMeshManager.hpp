#pragma once

#include "core/fanUnitTest.hpp"
#include "render/fanMeshManager.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/fanMesh.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestMeshManager : public UnitTest<UnitTestMeshManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestMeshManager::TestLoadMesh,           "Load" },
                    { &UnitTestMeshManager::TestMeshDelete,         "Delete" },
                    { &UnitTestMeshManager::TestMeshIndexing,       "Indexing" },
                    { &UnitTestMeshManager::TestClear,              "Clear" },
                    { &UnitTestMeshManager::TestResolveRscPtr,      "Resolve rsc ptr" },
                    { &UnitTestMeshManager::TestBuffersCreation,    "Buffers creation" },
                    { &UnitTestMeshManager::TestBuffersDestruction, "Buffers destruction" },
            };
        }
        void Create() override {}
        void Destroy() override
        {
            Device * device = nullptr;  // when testing we dont generate gpu buffers
            meshManager.Clear( *device );
        }

        MeshManager meshManager;

        void TestLoadMesh()
        {
            TEST_ASSERT( meshManager.Load( "toto" ) == nullptr );

            Mesh* meshCube = meshManager.GetOrLoad( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshCube != nullptr );
            Mesh* meshPlane = meshManager.Load( RenderGlobal::s_meshPlane );
            TEST_ASSERT( meshPlane != nullptr );

            TEST_ASSERT( meshManager.Get( RenderGlobal::s_meshCube ) == meshCube );
            TEST_ASSERT( meshManager.Get( RenderGlobal::s_meshPlane ) == meshPlane );

            Mesh* testMesh = new Mesh();
            TEST_ASSERT( testMesh->mExternallyOwned == false );
            const std::string testMeshName = "test_mesh";
            meshManager.Add( testMesh, testMeshName );
            TEST_ASSERT( meshManager.Get( testMeshName ) == testMesh );
            TEST_ASSERT( testMesh->mExternallyOwned == true );
        }

        void TestMeshIndexing()
        {
            Mesh* generatedMesh = new Mesh();
            const std::string generatedMeshName = "generated_mesh";

            Mesh* meshCube = meshManager.Load( RenderGlobal::s_meshCube );
            meshManager.Add( generatedMesh, generatedMeshName );
            Mesh* meshPlane = meshManager.Load( RenderGlobal::s_meshPlane );

            TEST_ASSERT( meshCube->mIndex == 0 );
            TEST_ASSERT( generatedMesh->mIndex == 1 );
            TEST_ASSERT( meshPlane->mIndex == 2 );

            meshManager.Remove( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshPlane->mIndex == 0 );
            TEST_ASSERT( generatedMesh->mIndex == 1 );
        }

        void TestMeshDelete()
        {
            meshManager.Load( RenderGlobal::s_meshCube );
            meshManager.Remove( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshManager.Get( RenderGlobal::s_meshCube ) == nullptr );
        }

        void TestClear() {
            meshManager.Load( RenderGlobal::s_meshCube );
            Device * device = nullptr;
            meshManager.Clear( *device );
            TEST_ASSERT( meshManager.Empty() );
        }

        void TestResolveRscPtr()
        {
            Mesh* meshCube = meshManager.Load( RenderGlobal::s_meshCube );
            MeshPtr rscPtr;
            TEST_ASSERT( ! rscPtr.IsValid() );
            rscPtr.Init( RenderGlobal::s_meshCube);
            meshManager.ResolvePtr( rscPtr );
            TEST_ASSERT( rscPtr.IsValid() );
            TEST_ASSERT( meshCube == rscPtr.GetResource() );
        }

        void TestBuffersCreation()
        {
            Mesh* mesh1 = new Mesh();
            Mesh* mesh2 = new Mesh();

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
            meshManager.Load( RenderGlobal::s_meshCube );
            meshManager.Remove( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshManager.DestroyListSize() == 6 );
            Device * device = nullptr;
            meshManager.Clear( * device );
            TEST_ASSERT( meshManager.DestroyListSize() == 0 );
        }
    };
}