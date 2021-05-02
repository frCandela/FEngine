#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector3.hpp"
#include "core/random/fanSimplexNoise.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
    struct RenderResources;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelTerrain : public EcsSingleton
    {
        ECS_SINGLETON( VoxelTerrain )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        static void GenerateTerrain( VoxelTerrain& _terrain );
        static void GenerateBlocks( VoxelTerrain& _terrain );
        static void GenerateMesh( VoxelTerrain& _terrain );
        static void InitializeTerrainMesh( VoxelTerrain& _terrain, RenderResources& _renderResources );

        static const int8_t  sCaseToNumPolys[256];
        static const int8_t  sEdgeConnectList[256][5][3];
        static const Vector3 sEdges[12];
        static const Vector3 sCorners[8];
        static const int     sChunkSize = 16;

        Mesh* mMesh;
        std::vector<Vertex> mVertices;
        bool                mGrid[sChunkSize][sChunkSize][sChunkSize];
        int                 mSeed;
        SimplexNoise        mSimplexNoise;
        Fixed               mAmplitude;
        Fixed               mFrequency;
    };
}