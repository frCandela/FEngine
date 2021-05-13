#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "render/resources/fanMeshManager.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestMeshManager : public UnitTest<UnitTestMeshManager>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return {
                    { &UnitTestMeshManager::TestLoadMesh,      "Load" },
                    { &UnitTestMeshManager::TestMeshDelete,    "Delete" },
                    { &UnitTestMeshManager::TestMeshIndexing,  "Indexing" },
                    { &UnitTestMeshManager::TestClear,         "Clear" },
                    { &UnitTestMeshManager::TestResolveRscPtr, "Resolve rsc ptr" },
                    { &UnitTestMeshManager::TestCreate,        "Create" },
                    { &UnitTestMeshManager::TestDestroy,       "Destroy" },
            };
        }
        void Create() override {}
        void Destroy() override
        {
            Device* device = nullptr;  // when testing we dont generate gpu buffers
            mMeshManager.Clear( *device );
        }

        MeshManager mMeshManager;

        void TestLoadMesh()
        {
            TEST_ASSERT( mMeshManager.Load( "toto" ) == nullptr );

            Mesh* meshCube = mMeshManager.GetOrLoad( RenderGlobal::sMeshCube );
            TEST_ASSERT( meshCube != nullptr );
            Mesh* meshPlane = mMeshManager.Load( RenderGlobal::sMeshPlane );
            TEST_ASSERT( meshPlane != nullptr );

            TEST_ASSERT( mMeshManager.Get( RenderGlobal::sMeshCube ) == meshCube );
            TEST_ASSERT( mMeshManager.Get( RenderGlobal::sMeshPlane ) == meshPlane );

            Mesh* testMesh = new Mesh();
            const std::string testMeshName = "test_mesh";
            mMeshManager.Add( testMesh, testMeshName );
            TEST_ASSERT( testMesh->mPath == testMeshName );
            TEST_ASSERT( mMeshManager.Get( testMeshName ) == testMesh );
        }

        void TestMeshIndexing()
        {
            Mesh* generatedMesh = new Mesh();
            const std::string generatedMeshName = "generated_mesh";

            Mesh* meshCube = mMeshManager.Load( RenderGlobal::sMeshCube );
            mMeshManager.Add( generatedMesh, generatedMeshName );
            Mesh* meshPlane = mMeshManager.Load( RenderGlobal::sMeshPlane );

            TEST_ASSERT( meshCube->mIndex == 0 );
            TEST_ASSERT( generatedMesh->mIndex == 1 );
            TEST_ASSERT( meshPlane->mIndex == 2 );

            mMeshManager.Remove( RenderGlobal::sMeshCube );
            TEST_ASSERT( meshPlane->mIndex == 0 );
            TEST_ASSERT( generatedMesh->mIndex == 1 );
        }

        void TestMeshDelete()
        {
            mMeshManager.Load( RenderGlobal::sMeshCube );
            mMeshManager.Remove( RenderGlobal::sMeshCube );
            TEST_ASSERT( mMeshManager.Get( RenderGlobal::sMeshCube ) == nullptr );
        }

        void TestClear()
        {
            mMeshManager.Load( RenderGlobal::sMeshCube );
            Device* device = nullptr;
            TEST_ASSERT( !mMeshManager.Empty() );
            mMeshManager.Clear( *device );
            TEST_ASSERT( mMeshManager.Empty() );
        }

        void TestResolveRscPtr()
        {
            Mesh* meshCube = mMeshManager.Load( RenderGlobal::sMeshCube );
            MeshPtr rscPtr;
            TEST_ASSERT( !rscPtr.IsValid() );
            rscPtr.Init( RenderGlobal::sMeshCube );
            mMeshManager.ResolvePtr( rscPtr );
            TEST_ASSERT( rscPtr.IsValid() );
            TEST_ASSERT( meshCube == rscPtr.GetResource() );
        }

        void TestCreate()
        {
            Mesh* mesh1 = new Mesh();
            Mesh* mesh2 = new Mesh();

            TEST_ASSERT( !mesh1->mBuffersOutdated );
            TEST_ASSERT( !mesh2->mBuffersOutdated );

            mMeshManager.Add( mesh1, "mesh1" );
            mMeshManager.Add( mesh2, "mesh2" );

            mesh1->mVertices = {};
            mesh1->LoadFromVertices();
            mesh2->LoadFromFile( RenderGlobal::sMeshCube );
            mesh2->mVertices.clear();

            TEST_ASSERT( mesh1->mBuffersOutdated );
            TEST_ASSERT( mesh2->mBuffersOutdated );

            Device* device = nullptr;
            mMeshManager.CreateNewMeshes( *device );

            TEST_ASSERT( !mesh1->mBuffersOutdated );
            TEST_ASSERT( !mesh2->mBuffersOutdated );
        }

        void TestDestroy()
        {
            mMeshManager.Load( RenderGlobal::sMeshCube );
            mMeshManager.Remove( RenderGlobal::sMeshCube );
            TEST_ASSERT( mMeshManager.DestroyListSize() == 1 );
            Device* device = nullptr;
            mMeshManager.Clear( *device );
            TEST_ASSERT( mMeshManager.DestroyListSize() == 0 );
        }
    };
}