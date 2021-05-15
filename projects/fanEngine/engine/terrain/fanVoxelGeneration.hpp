#pragma once

#include "core/random/fanSimplexNoise.hpp"
#include "engine/terrain/fanVoxelChunk.hpp"

#include "engine/terrain/fanNoiseOctave.hpp"

namespace fan
{
    struct VoxelTerrain;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelGenerator
    {
        void Initialize();

        static void GenerateBlocks( const VoxelTerrain& _terrain, VoxelChunk& _chunk );
        static void GenerateMesh( const VoxelTerrain& _terrain, VoxelChunk& _chunk, SubMesh& _mesh );
        static void GenerateVertices( const int _caseID, const Vector3 offset, std::vector<Vertex>& _vertices );

        int          mSeed;
        SimplexNoise mSimplexNoise;
        Fixed        mThreshold;
        bool         mClearSides;
        NoiseOctave  m2DOctave;
        NoiseOctave  m3DOctaves[2];

        static const int8_t  sCaseToNumPolys[256];
        static const int8_t  sEdgeConnectList[256][5][3];
        static const Vector3 sEdges[12];
        static const Vector3 sCorners[8];
    };
}