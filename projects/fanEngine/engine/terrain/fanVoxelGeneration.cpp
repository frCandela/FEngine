#include "engine/terrain/fanVoxelGeneration.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelGenerator::Initialize()
    {
        mSeed         = 42;
        mSimplexNoise = SimplexNoise( mSeed );
        m3DOctaves[0] = m3DOctaves[1] = m2DOctave = {};
        mClearSides = false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelGenerator::GenerateBlocks( const VoxelTerrain& _terrain, VoxelChunk& _chunk )
    {
        //ScopedTimer timerTotal( "terrain blocks generation" );

        _chunk.mIsGenerated = true;

        const VoxelGenerator& generator = _terrain.mGenerator;
        const glm::ivec3 maxPosition = VoxelChunk::sSize * _terrain.mSize - glm::ivec3 { 1, 1, 1 };
        const Vector3    maxSize     = Vector3( maxPosition );
        const Vector3    center      = Vector3( maxPosition ) / 2;
        for( int         x           = 0; x < VoxelChunk::sSize; ++x )
        {
            for( int y = 0; y < VoxelChunk::sSize; ++y )
            {
                for( int z = 0; z < VoxelChunk::sSize; ++z )
                {
                    const Vector3 chunkOffset    = VoxelChunk::sSize * Vector3( _chunk.mPosition.x, _chunk.mPosition.y, _chunk.mPosition.z );
                    const Vector3 globalPosition = chunkOffset + Vector3( x, y, z );

                    Fixed simplexVal2D  = generator.mSimplexNoise.Noise( globalPosition.x * generator.m2DOctave.mFrequency, globalPosition.z * generator.m2DOctave.mFrequency );
                    Fixed simplexVal3D0 = generator.mSimplexNoise.Noise( generator.m3DOctaves[0].mFrequency * globalPosition );
                    Fixed simplexVal3D1 = generator.mSimplexNoise.Noise( generator.m3DOctaves[1].mFrequency * globalPosition );

                    simplexVal2D *= generator.m2DOctave.mAmplitude;
                    simplexVal3D0 *= generator.m3DOctaves[0].mAmplitude;
                    simplexVal3D1 *= generator.m3DOctaves[1].mAmplitude;

                    Fixed heightRatio = globalPosition.y / maxPosition.y;
                    simplexVal2D *= 1 - generator.m2DOctave.mHeightWeight * heightRatio;
                    simplexVal3D0 -= generator.m3DOctaves[0].mHeightWeight * ( 2 * heightRatio - 1 );
                    simplexVal3D1 -= generator.m3DOctaves[1].mHeightWeight * ( 2 * heightRatio - 1 );

                    simplexVal2D  += generator.m2DOctave.mHeightOffset;
                    simplexVal3D0 += generator.m3DOctaves[0].mHeightOffset;
                    simplexVal3D1 += generator.m3DOctaves[1].mHeightOffset;

                    Fixed simplexVal = generator.m2DOctave.mWeight * ( 1 + FIXED( 0.5 ) * simplexVal2D - globalPosition.y / maxSize.y + generator.m2DOctave.mHeightOffset )
                                       + generator.m3DOctaves[0].mWeight * simplexVal3D0
                                       + generator.m3DOctaves[1].mWeight * simplexVal3D1;

                    _chunk( x, y, z ) = simplexVal;

                    if( !generator.mClearSides )
                    {
                        const glm::ivec3 gPos = VoxelChunk::sSize * glm::ivec3( _chunk.mPosition.x, _chunk.mPosition.y, _chunk.mPosition.z ) + glm::ivec3( x, y, z );
                        if( gPos.x == 0 || gPos.y == 0 || gPos.z == 0 || gPos.x == maxPosition.x || gPos.y == maxPosition.y || gPos.z == maxPosition.z )
                        {
                            _chunk( x, y, z ) = false;
                        }
                    }
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 VoxelGenerator::VertexInterpolate( const VoxelGenerator& _generator, const VoxelCellDensity& _cellDensity, int _edgeIndex )
    {
        Vector3 corner1, corner2;
        Fixed   density1, density2;
        switch( _edgeIndex )
        {
            case 0:
                density1 = _cellDensity.mBLB;
                density2 = _cellDensity.mTLB;
                corner1  = Vector3( 1, 0, 0 );
                corner2  = Vector3( 1, 1, 0 );
                break;
            case 1:
                density1 = _cellDensity.mTLB;
                density2 = _cellDensity.mTRB;
                corner1  = Vector3( 1, 1, 0 );
                corner2  = Vector3( 0, 1, 0 );
                break;
            case 2:
                density1 = _cellDensity.mTRB;
                density2 = _cellDensity.mBRB;
                corner1  = Vector3( 0, 1, 0 );
                corner2  = Vector3( 0, 0, 0 );
                break;
            case 3:
                density1 = _cellDensity.mBRB;
                density2 = _cellDensity.mBLB;
                corner1  = Vector3( 0, 0, 0 );
                corner2  = Vector3( 1, 0, 0 );
                break;
            case 4:
                density1 = _cellDensity.mBLF;
                density2 = _cellDensity.mTLF;
                corner1  = Vector3( 1, 0, 1 );
                corner2  = Vector3( 1, 1, 1 );
                break;
            case 5:
                density1 = _cellDensity.mTLF;
                density2 = _cellDensity.mTRF;
                corner1  = Vector3( 1, 1, 1 );
                corner2  = Vector3( 0, 1, 1 );
                break;
            case 6:
                density1 = _cellDensity.mTRF;
                density2 = _cellDensity.mBRF;
                corner1  = Vector3( 0, 1, 1 );
                corner2  = Vector3( 0, 0, 1 );
                break;
            case 7:
                density1 = _cellDensity.mBRF;
                density2 = _cellDensity.mBLF;
                corner1  = Vector3( 0, 0, 1 );
                corner2  = Vector3( 1, 0, 1 );
                break;
            case 8:
                density1 = _cellDensity.mBLB;
                density2 = _cellDensity.mBLF;
                corner1  = Vector3( 1, 0, 0 );
                corner2  = Vector3( 1, 0, 1 );
                break;
            case 9:
                density1 = _cellDensity.mTLB;
                density2 = _cellDensity.mTLF;
                corner1  = Vector3( 1, 1, 0 );
                corner2  = Vector3( 1, 1, 1 );
                break;
            case 10:
                density1 = _cellDensity.mTRB;
                density2 = _cellDensity.mTRF;
                corner1  = Vector3( 0, 1, 0 );
                corner2  = Vector3( 0, 1, 1 );
                break;
            case 11:
                density1 = _cellDensity.mBRB;
                density2 = _cellDensity.mBRF;
                corner1  = Vector3( 0, 0, 0 );
                corner2  = Vector3( 0, 0, 1 );
                break;
            default:
                fanAssert( false );
                density1 = density2 = Fixed( 0 );
                corner1  = corner2  = Vector3::sZero;
                return VoxelGenerator::sEdges[_edgeIndex];
                break;
        }

        (void)_generator;
        const Fixed scale = density1 / ( density1 - density2 );
        fanAssert( scale >= 0 && scale <= 1 );
        const Vector3 scaled   = corner1 + scale * ( corner2 - corner1 );
        const Vector3 unscaled = VoxelGenerator::sEdges[_edgeIndex];

        return _generator.mInterpolationScale * scaled + ( 1 - _generator.mInterpolationScale ) * unscaled;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelGenerator::GenerateVertices( const VoxelGenerator& _generator, const VoxelCellDensity _cellDensity, const Vector3 _offset, std::vector<Vertex>& _vertices )
    {
        const int caseID   = _cellDensity.CaseID();
        const int numPolys = sCaseToNumPolys[caseID];
        for( int  i        = 0; i < numPolys; ++i )
        {
            const int8_t* triangleIndices = sEdgeConnectList[caseID][i];
            Vector3   v0       = _offset + VertexInterpolate( _generator, _cellDensity, triangleIndices[0] );
            Vector3   v1       = _offset + VertexInterpolate( _generator, _cellDensity, triangleIndices[1] );
            Vector3   v2       = _offset + VertexInterpolate( _generator, _cellDensity, triangleIndices[2] );
            Vector3   fxNormal = Vector3::Cross( ( v1 - v2 ), ( v0 - v2 ) ).FastNormalized();
            glm::vec3 normal   = fxNormal.ToGlm();

            _vertices.push_back( { v0.ToGlm(), normal, Color( 1, 0, 0 ).ToGLM(), { 0, 0 } } );
            _vertices.push_back( { v2.ToGlm(), normal, Color( 1, 0, 0 ).ToGLM(), { 0, 0 } } );
            _vertices.push_back( { v1.ToGlm(), normal, Color( 1, 0, 0 ).ToGLM(), { 0, 0 } } );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GenerateLocalVoxels( const VoxelGenerator& _generator, VoxelChunk& _chunk, SubMesh& _mesh )
    {
        for( int x = 0; x < VoxelChunk::sSize - 1; ++x )
        {
            for( int y = 0; y < VoxelChunk::sSize - 1; ++y )
            {
                for( int z = 0; z < VoxelChunk::sSize - 1; ++z )
                {
                    const VoxelCellDensity cellDensity = { _chunk( x + 1, y + 0, z + 0 ),
                                                           _chunk( x + 1, y + 1, z + 0 ),
                                                           _chunk( x + 0, y + 1, z + 0 ),
                                                           _chunk( x + 0, y + 0, z + 0 ),
                                                           _chunk( x + 1, y + 0, z + 1 ),
                                                           _chunk( x + 1, y + 1, z + 1 ),
                                                           _chunk( x + 0, y + 1, z + 1 ),
                                                           _chunk( x + 0, y + 0, z + 1 ) };
                    VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( x, y, z ), _mesh.mVertices );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GenerateLeftVoxels( const VoxelGenerator& _generator, const VoxelChunk& _leftChunk, VoxelChunk& _chunk, SubMesh& _mesh )
    {
        for( int y = 0; y < VoxelChunk::sSize - 1; ++y )
        {
            for( int z = 0; z < VoxelChunk::sSize - 1; ++z )
            {
                const int              x           = VoxelChunk::sSize - 1;
                const VoxelCellDensity cellDensity = { _leftChunk( 0, y + 0, z + 0 ),
                                                       _leftChunk( 0, y + 1, z + 0 ),
                                                       _chunk( x, y + 1, z + 0 ),
                                                       _chunk( x, y + 0, z + 0 ),
                                                       _leftChunk( 0, y + 0, z + 1 ),
                                                       _leftChunk( 0, y + 1, z + 1 ),
                                                       _chunk( x, y + 1, z + 1 ),
                                                       _chunk( x, y + 0, z + 1 ) };
                VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( x, y, z ), _mesh.mVertices );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GenerateTopVoxels( const VoxelGenerator& _generator, const VoxelChunk& _topChunk, VoxelChunk& _chunk, SubMesh& _mesh )
    {
        for( int x = 0; x < VoxelChunk::sSize - 1; ++x )
        {
            for( int z = 0; z < VoxelChunk::sSize - 1; ++z )
            {
                const int              y           = VoxelChunk::sSize - 1;
                const VoxelCellDensity cellDensity = { _chunk( x + 1, y, z + 0 ),
                                                       _topChunk( x + 1, 0, z + 0 ),
                                                       _topChunk( x + 0, 0, z + 0 ),
                                                       _chunk( x + 0, y, z + 0 ),
                                                       _chunk( x + 1, y, z + 1 ),
                                                       _topChunk( x + 1, 0, z + 1 ),
                                                       _topChunk( x + 0, 0, z + 1 ),
                                                       _chunk( x + 0, y, z + 1 ) };
                VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( x, y, z ), _mesh.mVertices );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GenerateForwardVoxels( const VoxelGenerator& _generator, const VoxelChunk& _forwardChunk, VoxelChunk& _chunk, SubMesh& _mesh )
    {
        for( int x = 0; x < VoxelChunk::sSize - 1; ++x )
        {
            for( int y = 0; y < VoxelChunk::sSize - 1; ++y )
            {
                const int              z           = VoxelChunk::sSize - 1;
                const VoxelCellDensity cellDensity = { _chunk( x + 1, y + 0, z ),
                                                       _chunk( x + 1, y + 1, z ),
                                                       _chunk( x + 0, y + 1, z ),
                                                       _chunk( x + 0, y + 0, z ),
                                                       _forwardChunk( x + 1, y + 0, 0 ),
                                                       _forwardChunk( x + 1, y + 1, 0 ),
                                                       _forwardChunk( x + 0, y + 1, 0 ),
                                                       _forwardChunk( x + 0, y + 0, 0 ) };
                VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( x, y, z ), _mesh.mVertices );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GenerateLeftTopVoxels( const VoxelGenerator& _generator, const VoxelChunk& _leftChunk, const VoxelChunk& _topChunk, const VoxelChunk& _leftTopChunk, VoxelChunk& _chunk, SubMesh& _mesh )
    {

        for( int z = 0; z < VoxelChunk::sSize - 1; ++z )
        {
            const int              xy          = VoxelChunk::sSize - 1;
            const VoxelCellDensity cellDensity = { _leftChunk( 0, xy + 0, z + 0 ),
                                                   _leftTopChunk( 0, 0, z + 0 ),
                                                   _topChunk( xy, 0, z + 0 ),
                                                   _chunk( xy, xy, z + 0 ),
                                                   _leftChunk( 0, xy, z + 1 ),
                                                   _leftTopChunk( 0, 0, z + 1 ),
                                                   _topChunk( xy, 0, z + 1 ),
                                                   _chunk( xy, xy, z + 1 ) };
            VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( xy, xy, z ), _mesh.mVertices );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GenerateLeftForwardVoxels( const VoxelGenerator& _generator,
                                    const VoxelChunk& _leftChunk,
                                    const VoxelChunk& _forwardChunk,
                                    const VoxelChunk& _leftForwardChunk,
                                    VoxelChunk& _chunk,
                                    SubMesh& _mesh )
    {
        for( int y = 0; y < VoxelChunk::sSize - 1; ++y )
        {
            const int              xz          = VoxelChunk::sSize - 1;
            const VoxelCellDensity cellDensity = { _leftChunk( 0, y + 0, xz ),
                                                   _leftChunk( 0, y + 1, xz ),
                                                   _chunk( xz, y + 1, xz ),
                                                   _chunk( xz, y + 0, xz ),
                                                   _leftForwardChunk( 0, y + 0, 0 ),
                                                   _leftForwardChunk( 0, y + 1, 0 ),
                                                   _forwardChunk( xz, y + 1, 0 ),
                                                   _forwardChunk( xz, y + 0, 0 ) };
            VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( xz, y, xz ), _mesh.mVertices );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void
    GenerateTopForwardVoxels( const VoxelGenerator& _generator, const VoxelChunk& _topChunk, const VoxelChunk& _forwardChunk, const VoxelChunk& _topForwardChunk, VoxelChunk& _chunk, SubMesh& _mesh )
    {
        for( int x = 0; x < VoxelChunk::sSize - 1; ++x )
        {
            const int              yz          = VoxelChunk::sSize - 1;
            const VoxelCellDensity cellDensity = { _chunk( x + 1, yz, yz ),
                                                   _topChunk( x + 1, 0, yz ),
                                                   _topChunk( x + 0, 0, yz ),
                                                   _chunk( x + 0, yz, yz ),
                                                   _forwardChunk( x + 1, yz, 0 ),
                                                   _topForwardChunk( x + 1, 0, 0 ),
                                                   _topForwardChunk( x + 0, 0, 0 ),
                                                   _forwardChunk( x + 0, yz, 0 ) };
            VoxelGenerator::GenerateVertices( _generator, cellDensity, Vector3( x, yz, yz ), _mesh.mVertices );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelGenerator::GenerateMesh( const VoxelTerrain& _terrain, VoxelChunk& _chunk, SubMesh& _mesh )
    {
        // ScopedTimer         timerMesh( "gen voxel mesh" );
        _chunk.mIsMeshOutdated = false;

        _mesh.mVertices.resize( 0 );
        _mesh.mVertices.reserve( 30000 );

        const int leftChunkX    = _chunk.mPosition.x + 1;
        const int topChunkY     = _chunk.mPosition.y + 1;
        const int forwardChunkZ = _chunk.mPosition.z + 1;

        const VoxelChunk* leftChunk           = leftChunkX < _terrain.mSize.x ? &_terrain.GetChunk( { leftChunkX, _chunk.mPosition.y, _chunk.mPosition.z } ) : nullptr;
        const VoxelChunk* topChunk            = topChunkY < _terrain.mSize.y ? &_terrain.GetChunk( { _chunk.mPosition.x, topChunkY, _chunk.mPosition.z } ) : nullptr;
        const VoxelChunk* forwardChunk        = forwardChunkZ < _terrain.mSize.z ? &_terrain.GetChunk( { _chunk.mPosition.x, _chunk.mPosition.y, forwardChunkZ } ) : nullptr;
        const VoxelChunk* leftTopChunk        = leftChunk && topChunk ? &_terrain.GetChunk( { leftChunkX, topChunkY, _chunk.mPosition.z } ) : nullptr;
        const VoxelChunk* leftForwardChunk    = leftChunk && forwardChunk ? &_terrain.GetChunk( { leftChunkX, _chunk.mPosition.y, forwardChunkZ } ) : nullptr;
        const VoxelChunk* topForwardChunk     = topChunk && forwardChunk ? &_terrain.GetChunk( { _chunk.mPosition.x, topChunkY, forwardChunkZ } ) : nullptr;
        const VoxelChunk* topLeftForwardChunk = leftChunk && topChunk && forwardChunk ? &_terrain.GetChunk( { leftChunkX, topChunkY, forwardChunkZ } ) : nullptr;

        GenerateLocalVoxels( _terrain.mGenerator, _chunk, _mesh );

        if( leftChunk ){ GenerateLeftVoxels( _terrain.mGenerator, *leftChunk, _chunk, _mesh ); }
        if( topChunk ){ GenerateTopVoxels( _terrain.mGenerator, *topChunk, _chunk, _mesh ); }
        if( forwardChunk ){ GenerateForwardVoxels( _terrain.mGenerator, *forwardChunk, _chunk, _mesh ); }
        if( leftChunk && topChunk ){ GenerateLeftTopVoxels( _terrain.mGenerator, *leftChunk, *topChunk, *leftTopChunk, _chunk, _mesh ); }
        if( leftChunk && forwardChunk ){ GenerateLeftForwardVoxels( _terrain.mGenerator, *leftChunk, *forwardChunk, *leftForwardChunk, _chunk, _mesh ); }
        if( topChunk && forwardChunk ){ GenerateTopForwardVoxels( _terrain.mGenerator, *topChunk, *forwardChunk, *topForwardChunk, _chunk, _mesh ); }
        if( leftChunk && topChunk && forwardChunk )
        {
            const int              xyz         = VoxelChunk::sSize - 1;
            const VoxelCellDensity cellDensity = {
                    ( *leftChunk )( 0, xyz, xyz ),
                    ( *leftTopChunk )( 0, 0, xyz ),
                    ( *topChunk )( xyz, 0, xyz ),
                    ( _chunk )( xyz, xyz, xyz ),
                    ( *leftForwardChunk )( 0, xyz, 0 ),
                    ( *topLeftForwardChunk )( 0, 0, 0 ),
                    ( *topForwardChunk )( xyz, 0, 0 ),
                    ( *forwardChunk )( xyz, xyz, 0 ) };
            VoxelGenerator::GenerateVertices( _terrain.mGenerator, cellDensity, Vector3( xyz, xyz, xyz ), _mesh.mVertices );
        }

        glm::vec3 chunkOffset = VoxelChunk::sSize * _chunk.mPosition;
        glm::vec3 totalSize   = VoxelChunk::sSize * _terrain.mSize;
        for( Vertex& vertex : _mesh.mVertices )
        {
            glm::vec3 gPos = chunkOffset + vertex.mPos;
            vertex.mColor = gPos / totalSize;
        }

        _mesh.LoadFromVertices();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const Vector3 VoxelGenerator::sEdges[12] = {
            FIXED( 0.5 ) * Vector3( 2, 1, 0 ),
            FIXED( 0.5 ) * Vector3( 1, 2, 0 ),
            FIXED( 0.5 ) * Vector3( 0, 1, 0 ),
            FIXED( 0.5 ) * Vector3( 1, 0, 0 ),

            FIXED( 0.5 ) * Vector3( 2, 1, 2 ),
            FIXED( 0.5 ) * Vector3( 1, 2, 2 ),
            FIXED( 0.5 ) * Vector3( 0, 1, 2 ),
            FIXED( 0.5 ) * Vector3( 1, 0, 2 ),

            FIXED( 0.5 ) * Vector3( 2, 0, 1 ),
            FIXED( 0.5 ) * Vector3( 2, 2, 1 ),
            FIXED( 0.5 ) * Vector3( 0, 2, 1 ),
            FIXED( 0.5 ) * Vector3( 0, 0, 1 ),
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const Vector3 VoxelGenerator::sCorners[8] = { Vector3( 1, 0, 0 ),
                                                  Vector3( 1, 1, 0 ),
                                                  Vector3( 0, 1, 0 ),
                                                  Vector3( 0, 0, 0 ),
                                                  Vector3( 1, 0, 1 ),
                                                  Vector3( 1, 1, 1 ),
                                                  Vector3( 0, 1, 1 ),
                                                  Vector3( 0, 0, 1 ),
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const int8_t VoxelGenerator::sCaseToNumPolys[256] = {
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 2, 3, 4, 4, 3, 3, 4, 4, 3, 4, 5, 5, 2,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 4,
            2, 3, 3, 4, 3, 4, 2, 3, 3, 4, 4, 5, 4, 5, 3, 2, 3, 4, 4, 3, 4, 5, 3, 2, 4, 5, 5, 4, 5, 2, 4, 1,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 4, 3, 4, 4, 5, 3, 2, 4, 3, 4, 3, 5, 2,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 4, 3, 4, 4, 3, 4, 5, 5, 4, 4, 3, 5, 2, 5, 4, 2, 1,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 2, 3, 3, 2, 3, 4, 4, 5, 4, 5, 5, 2, 4, 3, 5, 4, 3, 2, 4, 1,
            3, 4, 4, 5, 4, 5, 3, 4, 4, 5, 5, 2, 3, 4, 2, 1, 2, 3, 3, 2, 3, 4, 2, 1, 3, 2, 4, 1, 2, 1, 1, 0 };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    const int8_t  VoxelGenerator::sEdgeConnectList[256][5][3] = {
            { { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  3 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  1,  9 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  8,  3 },  { 9,  8,  1 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  10 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  3 },  { 1,  2,  10 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  2,  10 }, { 0,  2,  9 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  8,  3 },  { 2,  10, 8 },  { 10, 9,  8 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  11, 2 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  11, 2 },  { 8,  11, 0 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  9,  0 },  { 2,  3,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  11, 2 },  { 1,  9,  11 }, { 9,  8,  11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  10, 1 },  { 11, 10, 3 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  10, 1 },  { 0,  8,  10 }, { 8,  11, 10 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  9,  0 },  { 3,  11, 9 },  { 11, 10, 9 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  8,  10 }, { 10, 8,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  7,  8 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  3,  0 },  { 7,  3,  4 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  1,  9 },  { 8,  4,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  1,  9 },  { 4,  7,  1 },  { 7,  3,  1 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  10 }, { 8,  4,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  4,  7 },  { 3,  0,  4 },  { 1,  2,  10 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  2,  10 }, { 9,  0,  2 },  { 8,  4,  7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  10, 9 },  { 2,  9,  7 },  { 2,  7,  3 },  { 7,  9,  4 },  { -1, -1, -1 } },
            { { 8,  4,  7 },  { 3,  11, 2 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 11, 4,  7 },  { 11, 2,  4 },  { 2,  0,  4 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  0,  1 },  { 8,  4,  7 },  { 2,  3,  11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  7,  11 }, { 9,  4,  11 }, { 9,  11, 2 },  { 9,  2,  1 },  { -1, -1, -1 } },
            { { 3,  10, 1 },  { 3,  11, 10 }, { 7,  8,  4 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  11, 10 }, { 1,  4,  11 }, { 1,  0,  4 },  { 7,  11, 4 },  { -1, -1, -1 } },
            { { 4,  7,  8 },  { 9,  0,  11 }, { 9,  11, 10 }, { 11, 0,  3 },  { -1, -1, -1 } },
            { { 4,  7,  11 }, { 4,  11, 9 },  { 9,  11, 10 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  5,  4 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  5,  4 },  { 0,  8,  3 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  5,  4 },  { 1,  5,  0 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  5,  4 },  { 8,  3,  5 },  { 3,  1,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  10 }, { 9,  5,  4 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  0,  8 },  { 1,  2,  10 }, { 4,  9,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  2,  10 }, { 5,  4,  2 },  { 4,  0,  2 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  10, 5 },  { 3,  2,  5 },  { 3,  5,  4 },  { 3,  4,  8 },  { -1, -1, -1 } },
            { { 9,  5,  4 },  { 2,  3,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  11, 2 },  { 0,  8,  11 }, { 4,  9,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  5,  4 },  { 0,  1,  5 },  { 2,  3,  11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  1,  5 },  { 2,  5,  8 },  { 2,  8,  11 }, { 4,  8,  5 },  { -1, -1, -1 } },
            { { 10, 3,  11 }, { 10, 1,  3 },  { 9,  5,  4 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  9,  5 },  { 0,  8,  1 },  { 8,  10, 1 },  { 8,  11, 10 }, { -1, -1, -1 } },
            { { 5,  4,  0 },  { 5,  0,  11 }, { 5,  11, 10 }, { 11, 0,  3 },  { -1, -1, -1 } },
            { { 5,  4,  8 },  { 5,  8,  10 }, { 10, 8,  11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  7,  8 },  { 5,  7,  9 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  3,  0 },  { 9,  5,  3 },  { 5,  7,  3 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  7,  8 },  { 0,  1,  7 },  { 1,  5,  7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  5,  3 },  { 3,  5,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  7,  8 },  { 9,  5,  7 },  { 10, 1,  2 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 1,  2 },  { 9,  5,  0 },  { 5,  3,  0 },  { 5,  7,  3 },  { -1, -1, -1 } },
            { { 8,  0,  2 },  { 8,  2,  5 },  { 8,  5,  7 },  { 10, 5,  2 },  { -1, -1, -1 } },
            { { 2,  10, 5 },  { 2,  5,  3 },  { 3,  5,  7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 7,  9,  5 },  { 7,  8,  9 },  { 3,  11, 2 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  5,  7 },  { 9,  7,  2 },  { 9,  2,  0 },  { 2,  7,  11 }, { -1, -1, -1 } },
            { { 2,  3,  11 }, { 0,  1,  8 },  { 1,  7,  8 },  { 1,  5,  7 },  { -1, -1, -1 } },
            { { 11, 2,  1 },  { 11, 1,  7 },  { 7,  1,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  5,  8 },  { 8,  5,  7 },  { 10, 1,  3 },  { 10, 3,  11 }, { -1, -1, -1 } },
            { { 5,  7,  0 },  { 5,  0,  9 },  { 7,  11, 0 },  { 1,  0,  10 }, { 11, 10, 0 } },
            { { 11, 10, 0 },  { 11, 0,  3 },  { 10, 5,  0 },  { 8,  0,  7 },  { 5,  7,  0 } },
            { { 11, 10, 5 },  { 7,  11, 5 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 6,  5 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  3 },  { 5,  10, 6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  0,  1 },  { 5,  10, 6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  8,  3 },  { 1,  9,  8 },  { 5,  10, 6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  6,  5 },  { 2,  6,  1 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  6,  5 },  { 1,  2,  6 },  { 3,  0,  8 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  6,  5 },  { 9,  0,  6 },  { 0,  2,  6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  9,  8 },  { 5,  8,  2 },  { 5,  2,  6 },  { 3,  2,  8 },  { -1, -1, -1 } },
            { { 2,  3,  11 }, { 10, 6,  5 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 11, 0,  8 },  { 11, 2,  0 },  { 10, 6,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  1,  9 },  { 2,  3,  11 }, { 5,  10, 6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  10, 6 },  { 1,  9,  2 },  { 9,  11, 2 },  { 9,  8,  11 }, { -1, -1, -1 } },
            { { 6,  3,  11 }, { 6,  5,  3 },  { 5,  1,  3 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  11 }, { 0,  11, 5 },  { 0,  5,  1 },  { 5,  11, 6 },  { -1, -1, -1 } },
            { { 3,  11, 6 },  { 0,  3,  6 },  { 0,  6,  5 },  { 0,  5,  9 },  { -1, -1, -1 } },
            { { 6,  5,  9 },  { 6,  9,  11 }, { 11, 9,  8 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  10, 6 },  { 4,  7,  8 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  3,  0 },  { 4,  7,  3 },  { 6,  5,  10 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  9,  0 },  { 5,  10, 6 },  { 8,  4,  7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 6,  5 },  { 1,  9,  7 },  { 1,  7,  3 },  { 7,  9,  4 },  { -1, -1, -1 } },
            { { 6,  1,  2 },  { 6,  5,  1 },  { 4,  7,  8 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  5 },  { 5,  2,  6 },  { 3,  0,  4 },  { 3,  4,  7 },  { -1, -1, -1 } },
            { { 8,  4,  7 },  { 9,  0,  5 },  { 0,  6,  5 },  { 0,  2,  6 },  { -1, -1, -1 } },
            { { 7,  3,  9 },  { 7,  9,  4 },  { 3,  2,  9 },  { 5,  9,  6 },  { 2,  6,  9 } },
            { { 3,  11, 2 },  { 7,  8,  4 },  { 10, 6,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  10, 6 },  { 4,  7,  2 },  { 4,  2,  0 },  { 2,  7,  11 }, { -1, -1, -1 } },
            { { 0,  1,  9 },  { 4,  7,  8 },  { 2,  3,  11 }, { 5,  10, 6 },  { -1, -1, -1 } },
            { { 9,  2,  1 },  { 9,  11, 2 },  { 9,  4,  11 }, { 7,  11, 4 },  { 5,  10, 6 } },
            { { 8,  4,  7 },  { 3,  11, 5 },  { 3,  5,  1 },  { 5,  11, 6 },  { -1, -1, -1 } },
            { { 5,  1,  11 }, { 5,  11, 6 },  { 1,  0,  11 }, { 7,  11, 4 },  { 0,  4,  11 } },
            { { 0,  5,  9 },  { 0,  6,  5 },  { 0,  3,  6 },  { 11, 6,  3 },  { 8,  4,  7 } },
            { { 6,  5,  9 },  { 6,  9,  11 }, { 4,  7,  9 },  { 7,  11, 9 },  { -1, -1, -1 } },
            { { 10, 4,  9 },  { 6,  4,  10 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  10, 6 },  { 4,  9,  10 }, { 0,  8,  3 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 0,  1 },  { 10, 6,  0 },  { 6,  4,  0 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  3,  1 },  { 8,  1,  6 },  { 8,  6,  4 },  { 6,  1,  10 }, { -1, -1, -1 } },
            { { 1,  4,  9 },  { 1,  2,  4 },  { 2,  6,  4 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  0,  8 },  { 1,  2,  9 },  { 2,  4,  9 },  { 2,  6,  4 },  { -1, -1, -1 } },
            { { 0,  2,  4 },  { 4,  2,  6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  3,  2 },  { 8,  2,  4 },  { 4,  2,  6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 4,  9 },  { 10, 6,  4 },  { 11, 2,  3 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  2 },  { 2,  8,  11 }, { 4,  9,  10 }, { 4,  10, 6 },  { -1, -1, -1 } },
            { { 3,  11, 2 },  { 0,  1,  6 },  { 0,  6,  4 },  { 6,  1,  10 }, { -1, -1, -1 } },
            { { 6,  4,  1 },  { 6,  1,  10 }, { 4,  8,  1 },  { 2,  1,  11 }, { 8,  11, 1 } },
            { { 9,  6,  4 },  { 9,  3,  6 },  { 9,  1,  3 },  { 11, 6,  3 },  { -1, -1, -1 } },
            { { 8,  11, 1 },  { 8,  1,  0 },  { 11, 6,  1 },  { 9,  1,  4 },  { 6,  4,  1 } },
            { { 3,  11, 6 },  { 3,  6,  0 },  { 0,  6,  4 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 6,  4,  8 },  { 11, 6,  8 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 7,  10, 6 },  { 7,  8,  10 }, { 8,  9,  10 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  7,  3 },  { 0,  10, 7 },  { 0,  9,  10 }, { 6,  7,  10 }, { -1, -1, -1 } },
            { { 10, 6,  7 },  { 1,  10, 7 },  { 1,  7,  8 },  { 1,  8,  0 },  { -1, -1, -1 } },
            { { 10, 6,  7 },  { 10, 7,  1 },  { 1,  7,  3 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  6 },  { 1,  6,  8 },  { 1,  8,  9 },  { 8,  6,  7 },  { -1, -1, -1 } },
            { { 2,  6,  9 },  { 2,  9,  1 },  { 6,  7,  9 },  { 0,  9,  3 },  { 7,  3,  9 } },
            { { 7,  8,  0 },  { 7,  0,  6 },  { 6,  0,  2 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 7,  3,  2 },  { 6,  7,  2 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  3,  11 }, { 10, 6,  8 },  { 10, 8,  9 },  { 8,  6,  7 },  { -1, -1, -1 } },
            { { 2,  0,  7 },  { 2,  7,  11 }, { 0,  9,  7 },  { 6,  7,  10 }, { 9,  10, 7 } },
            { { 1,  8,  0 },  { 1,  7,  8 },  { 1,  10, 7 },  { 6,  7,  10 }, { 2,  3,  11 } },
            { { 11, 2,  1 },  { 11, 1,  7 },  { 10, 6,  1 },  { 6,  7,  1 },  { -1, -1, -1 } },
            { { 8,  9,  6 },  { 8,  6,  7 },  { 9,  1,  6 },  { 11, 6,  3 },  { 1,  3,  6 } },
            { { 0,  9,  1 },  { 11, 6,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 7,  8,  0 },  { 7,  0,  6 },  { 3,  11, 0 },  { 11, 6,  0 },  { -1, -1, -1 } },
            { { 7,  11, 6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 7,  6,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  0,  8 },  { 11, 7,  6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  1,  9 },  { 11, 7,  6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  1,  9 },  { 8,  3,  1 },  { 11, 7,  6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 1,  2 },  { 6,  11, 7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  10 }, { 3,  0,  8 },  { 6,  11, 7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  9,  0 },  { 2,  10, 9 },  { 6,  11, 7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 6,  11, 7 },  { 2,  10, 3 },  { 10, 8,  3 },  { 10, 9,  8 },  { -1, -1, -1 } },
            { { 7,  2,  3 },  { 6,  2,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 7,  0,  8 },  { 7,  6,  0 },  { 6,  2,  0 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  7,  6 },  { 2,  3,  7 },  { 0,  1,  9 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  6,  2 },  { 1,  8,  6 },  { 1,  9,  8 },  { 8,  7,  6 },  { -1, -1, -1 } },
            { { 10, 7,  6 },  { 10, 1,  7 },  { 1,  3,  7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 7,  6 },  { 1,  7,  10 }, { 1,  8,  7 },  { 1,  0,  8 },  { -1, -1, -1 } },
            { { 0,  3,  7 },  { 0,  7,  10 }, { 0,  10, 9 },  { 6,  10, 7 },  { -1, -1, -1 } },
            { { 7,  6,  10 }, { 7,  10, 8 },  { 8,  10, 9 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 6,  8,  4 },  { 11, 8,  6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  6,  11 }, { 3,  0,  6 },  { 0,  4,  6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  6,  11 }, { 8,  4,  6 },  { 9,  0,  1 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  4,  6 },  { 9,  6,  3 },  { 9,  3,  1 },  { 11, 3,  6 },  { -1, -1, -1 } },
            { { 6,  8,  4 },  { 6,  11, 8 },  { 2,  10, 1 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  10 }, { 3,  0,  11 }, { 0,  6,  11 }, { 0,  4,  6 },  { -1, -1, -1 } },
            { { 4,  11, 8 },  { 4,  6,  11 }, { 0,  2,  9 },  { 2,  10, 9 },  { -1, -1, -1 } },
            { { 10, 9,  3 },  { 10, 3,  2 },  { 9,  4,  3 },  { 11, 3,  6 },  { 4,  6,  3 } },
            { { 8,  2,  3 },  { 8,  4,  2 },  { 4,  6,  2 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  4,  2 },  { 4,  6,  2 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  9,  0 },  { 2,  3,  4 },  { 2,  4,  6 },  { 4,  3,  8 },  { -1, -1, -1 } },
            { { 1,  9,  4 },  { 1,  4,  2 },  { 2,  4,  6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  1,  3 },  { 8,  6,  1 },  { 8,  4,  6 },  { 6,  10, 1 },  { -1, -1, -1 } },
            { { 10, 1,  0 },  { 10, 0,  6 },  { 6,  0,  4 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  6,  3 },  { 4,  3,  8 },  { 6,  10, 3 },  { 0,  3,  9 },  { 10, 9,  3 } },
            { { 10, 9,  4 },  { 6,  10, 4 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  9,  5 },  { 7,  6,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  3 },  { 4,  9,  5 },  { 11, 7,  6 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  0,  1 },  { 5,  4,  0 },  { 7,  6,  11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 11, 7,  6 },  { 8,  3,  4 },  { 3,  5,  4 },  { 3,  1,  5 },  { -1, -1, -1 } },
            { { 9,  5,  4 },  { 10, 1,  2 },  { 7,  6,  11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 6,  11, 7 },  { 1,  2,  10 }, { 0,  8,  3 },  { 4,  9,  5 },  { -1, -1, -1 } },
            { { 7,  6,  11 }, { 5,  4,  10 }, { 4,  2,  10 }, { 4,  0,  2 },  { -1, -1, -1 } },
            { { 3,  4,  8 },  { 3,  5,  4 },  { 3,  2,  5 },  { 10, 5,  2 },  { 11, 7,  6 } },
            { { 7,  2,  3 },  { 7,  6,  2 },  { 5,  4,  9 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  5,  4 },  { 0,  8,  6 },  { 0,  6,  2 },  { 6,  8,  7 },  { -1, -1, -1 } },
            { { 3,  6,  2 },  { 3,  7,  6 },  { 1,  5,  0 },  { 5,  4,  0 },  { -1, -1, -1 } },
            { { 6,  2,  8 },  { 6,  8,  7 },  { 2,  1,  8 },  { 4,  8,  5 },  { 1,  5,  8 } },
            { { 9,  5,  4 },  { 10, 1,  6 },  { 1,  7,  6 },  { 1,  3,  7 },  { -1, -1, -1 } },
            { { 1,  6,  10 }, { 1,  7,  6 },  { 1,  0,  7 },  { 8,  7,  0 },  { 9,  5,  4 } },
            { { 4,  0,  10 }, { 4,  10, 5 },  { 0,  3,  10 }, { 6,  10, 7 },  { 3,  7,  10 } },
            { { 7,  6,  10 }, { 7,  10, 8 },  { 5,  4,  10 }, { 4,  8,  10 }, { -1, -1, -1 } },
            { { 6,  9,  5 },  { 6,  11, 9 },  { 11, 8,  9 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  6,  11 }, { 0,  6,  3 },  { 0,  5,  6 },  { 0,  9,  5 },  { -1, -1, -1 } },
            { { 0,  11, 8 },  { 0,  5,  11 }, { 0,  1,  5 },  { 5,  6,  11 }, { -1, -1, -1 } },
            { { 6,  11, 3 },  { 6,  3,  5 },  { 5,  3,  1 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  10 }, { 9,  5,  11 }, { 9,  11, 8 },  { 11, 5,  6 },  { -1, -1, -1 } },
            { { 0,  11, 3 },  { 0,  6,  11 }, { 0,  9,  6 },  { 5,  6,  9 },  { 1,  2,  10 } },
            { { 11, 8,  5 },  { 11, 5,  6 },  { 8,  0,  5 },  { 10, 5,  2 },  { 0,  2,  5 } },
            { { 6,  11, 3 },  { 6,  3,  5 },  { 2,  10, 3 },  { 10, 5,  3 },  { -1, -1, -1 } },
            { { 5,  8,  9 },  { 5,  2,  8 },  { 5,  6,  2 },  { 3,  8,  2 },  { -1, -1, -1 } },
            { { 9,  5,  6 },  { 9,  6,  0 },  { 0,  6,  2 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  5,  8 },  { 1,  8,  0 },  { 5,  6,  8 },  { 3,  8,  2 },  { 6,  2,  8 } },
            { { 1,  5,  6 },  { 2,  1,  6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  3,  6 },  { 1,  6,  10 }, { 3,  8,  6 },  { 5,  6,  9 },  { 8,  9,  6 } },
            { { 10, 1,  0 },  { 10, 0,  6 },  { 9,  5,  0 },  { 5,  6,  0 },  { -1, -1, -1 } },
            { { 0,  3,  8 },  { 5,  6,  10 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 5,  6 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 11, 5,  10 }, { 7,  5,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 11, 5,  10 }, { 11, 7,  5 },  { 8,  3,  0 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  11, 7 },  { 5,  10, 11 }, { 1,  9,  0 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 10, 7,  5 },  { 10, 11, 7 },  { 9,  8,  1 },  { 8,  3,  1 },  { -1, -1, -1 } },
            { { 11, 1,  2 },  { 11, 7,  1 },  { 7,  5,  1 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  3 },  { 1,  2,  7 },  { 1,  7,  5 },  { 7,  2,  11 }, { -1, -1, -1 } },
            { { 9,  7,  5 },  { 9,  2,  7 },  { 9,  0,  2 },  { 2,  11, 7 },  { -1, -1, -1 } },
            { { 7,  5,  2 },  { 7,  2,  11 }, { 5,  9,  2 },  { 3,  2,  8 },  { 9,  8,  2 } },
            { { 2,  5,  10 }, { 2,  3,  5 },  { 3,  7,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  2,  0 },  { 8,  5,  2 },  { 8,  7,  5 },  { 10, 2,  5 },  { -1, -1, -1 } },
            { { 9,  0,  1 },  { 5,  10, 3 },  { 5,  3,  7 },  { 3,  10, 2 },  { -1, -1, -1 } },
            { { 9,  8,  2 },  { 9,  2,  1 },  { 8,  7,  2 },  { 10, 2,  5 },  { 7,  5,  2 } },
            { { 1,  3,  5 },  { 3,  7,  5 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  7 },  { 0,  7,  1 },  { 1,  7,  5 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  0,  3 },  { 9,  3,  5 },  { 5,  3,  7 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  8,  7 },  { 5,  9,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  8,  4 },  { 5,  10, 8 },  { 10, 11, 8 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 5,  0,  4 },  { 5,  11, 0 },  { 5,  10, 11 }, { 11, 3,  0 },  { -1, -1, -1 } },
            { { 0,  1,  9 },  { 8,  4,  10 }, { 8,  10, 11 }, { 10, 4,  5 },  { -1, -1, -1 } },
            { { 10, 11, 4 },  { 10, 4,  5 },  { 11, 3,  4 },  { 9,  4,  1 },  { 3,  1,  4 } },
            { { 2,  5,  1 },  { 2,  8,  5 },  { 2,  11, 8 },  { 4,  5,  8 },  { -1, -1, -1 } },
            { { 0,  4,  11 }, { 0,  11, 3 },  { 4,  5,  11 }, { 2,  11, 1 },  { 5,  1,  11 } },
            { { 0,  2,  5 },  { 0,  5,  9 },  { 2,  11, 5 },  { 4,  5,  8 },  { 11, 8,  5 } },
            { { 9,  4,  5 },  { 2,  11, 3 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  5,  10 }, { 3,  5,  2 },  { 3,  4,  5 },  { 3,  8,  4 },  { -1, -1, -1 } },
            { { 5,  10, 2 },  { 5,  2,  4 },  { 4,  2,  0 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  10, 2 },  { 3,  5,  10 }, { 3,  8,  5 },  { 4,  5,  8 },  { 0,  1,  9 } },
            { { 5,  10, 2 },  { 5,  2,  4 },  { 1,  9,  2 },  { 9,  4,  2 },  { -1, -1, -1 } },
            { { 8,  4,  5 },  { 8,  5,  3 },  { 3,  5,  1 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  4,  5 },  { 1,  0,  5 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 8,  4,  5 },  { 8,  5,  3 },  { 9,  0,  5 },  { 0,  3,  5 },  { -1, -1, -1 } },
            { { 9,  4,  5 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  11, 7 },  { 4,  9,  11 }, { 9,  10, 11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  8,  3 },  { 4,  9,  7 },  { 9,  11, 7 },  { 9,  10, 11 }, { -1, -1, -1 } },
            { { 1,  10, 11 }, { 1,  11, 4 },  { 1,  4,  0 },  { 7,  4,  11 }, { -1, -1, -1 } },
            { { 3,  1,  4 },  { 3,  4,  8 },  { 1,  10, 4 },  { 7,  4,  11 }, { 10, 11, 4 } },
            { { 4,  11, 7 },  { 9,  11, 4 },  { 9,  2,  11 }, { 9,  1,  2 },  { -1, -1, -1 } },
            { { 9,  7,  4 },  { 9,  11, 7 },  { 9,  1,  11 }, { 2,  11, 1 },  { 0,  8,  3 } },
            { { 11, 7,  4 },  { 11, 4,  2 },  { 2,  4,  0 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 11, 7,  4 },  { 11, 4,  2 },  { 8,  3,  4 },  { 3,  2,  4 },  { -1, -1, -1 } },
            { { 2,  9,  10 }, { 2,  7,  9 },  { 2,  3,  7 },  { 7,  4,  9 },  { -1, -1, -1 } },
            { { 9,  10, 7 },  { 9,  7,  4 },  { 10, 2,  7 },  { 8,  7,  0 },  { 2,  0,  7 } },
            { { 3,  7,  10 }, { 3,  10, 2 },  { 7,  4,  10 }, { 1,  10, 0 },  { 4,  0,  10 } },
            { { 1,  10, 2 },  { 8,  7,  4 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  9,  1 },  { 4,  1,  7 },  { 7,  1,  3 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  9,  1 },  { 4,  1,  7 },  { 0,  8,  1 },  { 8,  7,  1 },  { -1, -1, -1 } },
            { { 4,  0,  3 },  { 7,  4,  3 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 4,  8,  7 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  10, 8 },  { 10, 11, 8 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  0,  9 },  { 3,  9,  11 }, { 11, 9,  10 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  1,  10 }, { 0,  10, 8 },  { 8,  10, 11 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  1,  10 }, { 11, 3,  10 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  2,  11 }, { 1,  11, 9 },  { 9,  11, 8 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  0,  9 },  { 3,  9,  11 }, { 1,  2,  9 },  { 2,  11, 9 },  { -1, -1, -1 } },
            { { 0,  2,  11 }, { 8,  0,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 3,  2,  11 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  3,  8 },  { 2,  8,  10 }, { 10, 8,  9 },  { -1, -1, -1 }, { -1, -1, -1 } },
            { { 9,  10, 2 },  { 0,  9,  2 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 2,  3,  8 },  { 2,  8,  10 }, { 0,  1,  8 },  { 1,  10, 8 },  { -1, -1, -1 } },
            { { 1,  10, 2 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 1,  3,  8 },  { 9,  1,  8 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  9,  1 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { 0,  3,  8 },  { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
            { { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 }, { -1, -1, -1 } },
    };
}
       