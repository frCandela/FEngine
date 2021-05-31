#include "engine/terrain/fanVoxelTerrain.hpp"
#include <fstream>
#include "core/resources/fanResources.hpp"
#include "core/memory/fanBase64.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanMaterial.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/fanEngineTags.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save       = &VoxelTerrain::Save;
        _info.load       = &VoxelTerrain::Load;
        _info.saveBinary = &VoxelTerrain::SaveBinary;
        _info.loadBinary = &VoxelTerrain::LoadBinary;
        _info.destroy    = &VoxelTerrain::Destroy;
        _info.postInit   = &VoxelTerrain::PostInit;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _singleton );
        voxelTerrain.mTerrainHandle = 0;
        voxelTerrain.mSize          = { 0, 0, 0 };
        voxelTerrain.mChunks        = nullptr;
        voxelTerrain.mIsInitialized = false;
        voxelTerrain.mGenerator.Initialize();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::PostInit( EcsWorld& _world, EcsSingleton& _singleton )
    {
        VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _singleton );
        InitializeTerrain( _world, voxelTerrain );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Destroy( EcsWorld& _world, EcsSingleton& _singleton )
    {
        VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _singleton );
        ClearTerrain( _world, voxelTerrain );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::InitializeTerrain( EcsWorld& _world, VoxelTerrain& _terrain )
    {
        if( _terrain.mSize.x <= 0 || _terrain.mSize.y <= 0 || _terrain.mSize.z <= 0 ){ return; }

        Application& app = _world.GetSingleton<Application>();
        ResourcePtr<Texture> texture = app.mResources->GetOrLoad<Texture>( "_default/texture/white.png" );

        Scene    & scene       = _world.GetSingleton<Scene>();
        SceneNode& terrainRoot = scene.CreateSceneNode( "terrain", &scene.GetRootNode() );
        terrainRoot.AddFlag( SceneNode::NoSave );
        _terrain.mTerrainHandle = terrainRoot.mHandle;

        if( _terrain.mChunks == nullptr )
        {
            _terrain.mChunks = new VoxelChunk[_terrain.mSize.x * _terrain.mSize.y * _terrain.mSize.z] {};
        }

        glm::ivec3 position;
        for( position.x         = 0; position.x < _terrain.mSize.x; position.x++ )
        {
            for( position.y = 0; position.y < _terrain.mSize.y; position.y++ )
            {
                for( position.z = 0; position.z < _terrain.mSize.z; position.z++ )
                {
                    const std::string chunkName = "chunk_" + std::to_string( position.x ) + "_" + std::to_string( position.y ) + "_" + std::to_string( position.z );
                    SceneNode& chunkNode = scene.CreateSceneNode( chunkName, &terrainRoot );
                    EcsEntity entity = _world.GetEntity( chunkNode.mHandle );
                    _world.AddTag<TagTerrain>( entity );

                    VoxelChunk& chunk = _terrain.GetChunk( position );
                    chunk.mPosition = position;
                    chunk.mHandle   = chunkNode.mHandle;

                    Transform& transform = _world.AddComponent<Transform>( entity );
                    transform.mPosition = Fixed( VoxelChunk::sSize ) * Vector3( position.x, position.y, position.z );

                    fan::ResourcePtr<fan::Mesh> mesh = app.mResources->Get<Mesh>( chunkName );
                    if( !mesh )
                    {
                        mesh = app.mResources->Add<Mesh>( new Mesh, chunkName );
                    }
                    MeshRenderer& renderer = _world.AddComponent<MeshRenderer>( entity );
                    mesh->mSubMeshes.resize( 1 );
                    mesh->mSubMeshes[0].mOptimizeVertices = true;
                    mesh->mSubMeshes[0].mHostVisible      = true;
                    mesh->mPath                           = chunkName;
                    renderer.mMesh                        = mesh;

                    Material& material = _world.AddComponent<Material>( entity );
                    material.mMaterials[0].mTexture = texture;
                }
            }
        }
        _terrain.mIsInitialized = true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::ClearTerrain( EcsWorld& _world, VoxelTerrain& _terrain )
    {
        if( _terrain.mTerrainHandle == 0 || !_world.HandleExists( _terrain.mTerrainHandle ) )
        {
            return;
        }

        for( int i = 0; i < _terrain.mSize.x * _terrain.mSize.y * _terrain.mSize.z; i++ )
        {
            VoxelChunk& chunk = _terrain.mChunks[i];
            if( chunk.mHandle != 0 && _world.HandleExists( chunk.mHandle ) )
            {
                EcsEntity entity = _world.GetEntity( chunk.mHandle );
                MeshRenderer& renderer = _world.AddComponent<MeshRenderer>( entity );
                renderer.mMesh = nullptr;
                _world.Kill( entity );
            }
        }
        _world.Kill( _world.GetEntity( _terrain.mTerrainHandle ) );
        delete[] _terrain.mChunks;
        _terrain.mChunks        = nullptr;
        _terrain.mSize          = { 4, 2, 4 };
        _terrain.mIsInitialized = false;
        _terrain.mTerrainHandle = 0;
    }

    //==================================================================================================================================================================================================
    // load a number of terrain chunks per frame
    //==================================================================================================================================================================================================
    void VoxelTerrain::StepLoadTerrain( EcsWorld& _world, const int _chunksPerFrame )
    {
        VoxelTerrain& terrain   = _world.GetSingleton<VoxelTerrain>();
        Resources   & resources = *_world.GetSingleton<Application>().mResources;
        if( !terrain.mIsInitialized ){ return; }

        for( int iteration = 0; iteration < _chunksPerFrame; ++iteration )
        {
            // load/generate blocks
            bool     generatedBlocks = false;
            for( int i               = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; i++ )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                if( !chunk.mIsGenerated )
                {
                    VoxelGenerator::GenerateBlocks( terrain, chunk );
                    generatedBlocks = true;
                    break;
                }
            }
            if( generatedBlocks ){ continue; }

            // generate mesh
            for( int i = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; i++ )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                if( chunk.mIsMeshOutdated )
                {
                    EcsEntity entity = _world.GetEntity( chunk.mHandle );
                    MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entity );
                    SceneNode   & sceneNode    = _world.GetComponent<SceneNode>( entity );
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
                    VoxelGenerator::GenerateMesh( terrain, chunk, ( meshRenderer.mMesh )->mSubMeshes[0] );
                    meshRenderer.mMesh->GenerateBoundingVolumes();
                    resources.SetDirty( meshRenderer.mMesh->mGUID );
                    break;
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Save( const EcsSingleton& _component, Json& _json )
    {
        const VoxelTerrain& terrain = static_cast<const VoxelTerrain&>( _component );
        Serializable::SaveInt3( _json, "size", terrain.mSize );
        Serializable::SaveInt( _json, "seed", terrain.mGenerator.mSeed );
        Serializable::SaveFixed( _json, "interpolation_scale", terrain.mGenerator.mInterpolationScale );
        Serializable::SaveBool( _json, "clear_sides", terrain.mGenerator.mClearSides );
        NoiseOctave::Save( _json, "3d_octave_0", terrain.mGenerator.m3DOctaves[0] );
        NoiseOctave::Save( _json, "3d_octave_1", terrain.mGenerator.m3DOctaves[1] );
        NoiseOctave::Save( _json, "2d_octave", terrain.mGenerator.m2DOctave );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Load( EcsSingleton& _component, const Json& _json )
    {
        VoxelTerrain& terrain = static_cast<VoxelTerrain&>( _component );
        Serializable::LoadInt3( _json, "size", terrain.mSize );
        Serializable::LoadInt( _json, "seed", terrain.mGenerator.mSeed );
        Serializable::LoadFixed( _json, "interpolation_scale", terrain.mGenerator.mInterpolationScale );
        Serializable::LoadBool( _json, "clear_sides", terrain.mGenerator.mClearSides );
        NoiseOctave::Load( _json, "3d_octave_0", terrain.mGenerator.m3DOctaves[0] );
        NoiseOctave::Load( _json, "3d_octave_1", terrain.mGenerator.m3DOctaves[1] );
        NoiseOctave::Load( _json, "2d_octave", terrain.mGenerator.m2DOctave );

        fanAssert( terrain.mChunks == nullptr );
        terrain.mChunks = new VoxelChunk[terrain.mSize.x * terrain.mSize.y * terrain.mSize.z] {};
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::VoxelTerrain::SaveBinary( const EcsSingleton& _singleton, const char* _path )
    {
        const VoxelTerrain& terrain = static_cast<const VoxelTerrain&>(_singleton);
        std::ofstream outStream( std::string( _path ) + ".terrain", std::ios::binary );
        if( outStream.is_open() )
        {
            const int sizeChunk = VoxelChunk::sSize * VoxelChunk::sSize * VoxelChunk::sSize * sizeof( Fixed );
            for( int  i         = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; ++i )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                outStream.write( (const char*)chunk.mVoxels, sizeChunk );
            }
            outStream.close();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::VoxelTerrain::LoadBinary( EcsSingleton& _singleton, const char* _path )
    {
        VoxelTerrain& terrain = static_cast<VoxelTerrain&>(_singleton);
        std::ifstream inStream( std::string( _path ) + ".terrain", std::ios::binary );
        if( inStream.is_open() && inStream.good() )
        {
            const int numChunks = terrain.mSize.x * terrain.mSize.y * terrain.mSize.z;
            const int sizeChunk = VoxelChunk::sSize * VoxelChunk::sSize * VoxelChunk::sSize * sizeof( Fixed );
            for( int  i         = 0; i < numChunks; ++i )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                inStream.read( (char*)chunk.mVoxels, sizeChunk );
                chunk.mIsGenerated = true;
            }
        }
    }
}