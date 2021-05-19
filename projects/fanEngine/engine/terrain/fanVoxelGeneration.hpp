#pragma once

#include "core/random/fanSimplexNoise.hpp"
#include "engine/terrain/fanVoxelChunk.hpp"

#include "engine/terrain/fanNoiseOctave.hpp"

namespace fan
{
    struct VoxelTerrain;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelCellDensity
    {
        Fixed mBLB; // bot left  back
        Fixed mTLB;
        Fixed mTRB;
        Fixed mBRB; // bot right  back
        Fixed mBLF;
        Fixed mTLF; //top left forward
        Fixed mTRF;
        Fixed mBRF;
        int CaseID() const
        {
            return ( mBLB > 0 ) << 0 |
                   ( mTLB > 0 ) << 1 |
                   ( mTRB > 0 ) << 2 |
                   ( mBRB > 0 ) << 3 |
                   ( mBLF > 0 ) << 4 |
                   ( mTLF > 0 ) << 5 |
                   ( mTRF > 0 ) << 6 |
                   ( mBRF > 0 ) << 7;
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct VoxelGenerator
    {
        void Initialize();

        static void GenerateBlocks( const VoxelTerrain& _terrain, VoxelChunk& _chunk );
        static void GenerateMesh( const VoxelTerrain& _terrain, VoxelChunk& _chunk, SubMesh& _mesh );
        static void GenerateVertices(  const VoxelGenerator& _generator, const VoxelCellDensity _cellDensity, const Vector3 offset, std::vector<Vertex>& _vertices );
        static Vector3 VertexInterpolate( const VoxelGenerator& _generator, const VoxelCellDensity& _cellDensity, int _edgeIndex );

        int          mSeed;
        SimplexNoise mSimplexNoise;
        Fixed        mThreshold;
        Fixed        mInterpolationScale;
        bool         mClearSides;
        NoiseOctave  m2DOctave;
        NoiseOctave  m3DOctaves[2];

        static const int8_t  sCaseToNumPolys[256];
        static const int8_t  sEdgeConnectList[256][5][3];
        static const Vector3 sEdges[12];
        static const Vector3 sCorners[8];
    };
}