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
            TEST_ASSERT( meshManager.LoadMesh( "toto" ) == nullptr );

            Mesh* meshCube = meshManager.GetOrLoad( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshCube != nullptr );
            Mesh* meshPlane = meshManager.LoadMesh( RenderGlobal::s_meshPlane );
            TEST_ASSERT( meshPlane != nullptr );

            TEST_ASSERT( meshManager.GetMesh( RenderGlobal::s_meshCube ) == meshCube );
            TEST_ASSERT( meshManager.GetMesh( RenderGlobal::s_meshPlane ) == meshPlane );

            Mesh* testMesh = new Mesh();
            TEST_ASSERT( testMesh->mExternallyOwned == false );
            const std::string testMeshName = "test_mesh";
            meshManager.AddMesh( testMesh, testMeshName );
            TEST_ASSERT( meshManager.GetMesh( testMeshName ) == testMesh );
            TEST_ASSERT( testMesh->mExternallyOwned == true );
        }

        void TestMeshIndexing()
        {
            Mesh* generatedMesh = new Mesh();
            const std::string generatedMeshName = "generated_mesh";

            Mesh* meshCube = meshManager.LoadMesh( RenderGlobal::s_meshCube );
            meshManager.AddMesh( generatedMesh, generatedMeshName );
            Mesh* meshPlane = meshManager.LoadMesh( RenderGlobal::s_meshPlane );

            TEST_ASSERT( meshCube->mIndex == 0 );
            TEST_ASSERT( generatedMesh->mIndex == 1 );
            TEST_ASSERT( meshPlane->mIndex == 2 );

            meshManager.Remove( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshPlane->mIndex == 0 );
            TEST_ASSERT( generatedMesh->mIndex == 1 );
        }

        void TestMeshDelete()
        {
            meshManager.LoadMesh( RenderGlobal::s_meshCube );
            meshManager.Remove( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshManager.GetMesh( RenderGlobal::s_meshCube ) == nullptr );
        }

        void TestClear() {
            meshManager.LoadMesh( RenderGlobal::s_meshCube );
            Device * device = nullptr;
            meshManager.Clear( *device );
            TEST_ASSERT( meshManager.Empty() );
        }

        void TestResolveRscPtr()
        {
            Mesh* meshCube = meshManager.LoadMesh( RenderGlobal::s_meshCube );
            MeshPtr rscPtr;
            TEST_ASSERT( ! rscPtr.IsValid() );
            rscPtr.Init( RenderGlobal::s_meshCube);
            meshManager.ResolvePtr( rscPtr );
            TEST_ASSERT( rscPtr.IsValid() );
            TEST_ASSERT( meshCube == rscPtr.GetResource() );
        }

        void TestBuffersDestruction()
        {
            meshManager.LoadMesh( RenderGlobal::s_meshCube );
            meshManager.Remove( RenderGlobal::s_meshCube );
            TEST_ASSERT( meshManager.GetCountBuffersPendingDestruction() == 6 );
            Device * device = nullptr;
            meshManager.Clear( * device );
            TEST_ASSERT( meshManager.GetCountBuffersPendingDestruction() == 0 );
        }
    };
}