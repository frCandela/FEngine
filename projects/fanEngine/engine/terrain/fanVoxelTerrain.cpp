#include "core/time/fanScopedTimer.hpp"
#include "core/memory/fanSerializable.hpp"
#include "engine/singletons/fanRenderResources.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/physics/fanFxTransform.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanMaterial.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"
#include "render/resources/fanTextureManager.hpp"
#include "render/resources/fanMeshManager.hpp"
#include "core/memory/fanBase64.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save     = &VoxelTerrain::Save;
        _info.load     = &VoxelTerrain::Load;
        _info.destroy  = &VoxelTerrain::Destroy;
        _info.postInit = &VoxelTerrain::PostInit;
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

        RenderResources& renderResources = _world.GetSingleton<RenderResources>();
        Texture        * texture         = renderResources.mTextureManager->GetOrLoad( "_default/texture/white.png" );

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

                    VoxelChunk& chunk = _terrain.GetChunk( position );
                    chunk.mPosition = position;
                    chunk.mHandle   = chunkNode.mHandle;

                    FxTransform& transform = _world.AddComponent<FxTransform>( entity );
                    transform.mPosition = Fixed( VoxelChunk::sSize ) * Vector3( position.x, position.y, position.z );

                    Mesh* mesh = renderResources.mMeshManager->Get( chunkName );
                    if( !mesh ){ mesh = new Mesh; }
                    MeshRenderer& renderer = _world.AddComponent<MeshRenderer>( entity );
                    mesh->mOptimizeVertices = false;
                    mesh->mAutoUpdateHull   = false;
                    mesh->mHostVisible      = true;
                    renderer.mMesh          = mesh;
                    renderResources.mMeshManager->Add( mesh, chunkName );

                    Material& material = _world.AddComponent<Material>( entity );
                    material.mTexture = texture;
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
    //==================================================================================================================================================================================================
    void VoxelTerrain::Save( const EcsSingleton& _component, Json& _json )
    {
        const VoxelTerrain& terrain = static_cast<const VoxelTerrain&>( _component );
        Serializable::SaveInt3( _json, "size", terrain.mSize );
        Serializable::SaveInt( _json, "seed", terrain.mGenerator.mSeed );
        Serializable::SaveFixed( _json, "threshold", terrain.mGenerator.mThreshold );
        Serializable::SaveBool( _json, "clear_sides", terrain.mGenerator.mClearSides );
        NoiseOctave::Save( _json, "3d_octave_0", terrain.mGenerator.m3DOctaves[0] );
        NoiseOctave::Save( _json, "3d_octave_1", terrain.mGenerator.m3DOctaves[1] );
        NoiseOctave::Save( _json, "2d_octave", terrain.mGenerator.m2DOctave );

        Json& chunksJson = _json["chunks"];
        for( int i = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; ++i )
        {
            VoxelChunk& chunk = terrain.mChunks[i];
            chunksJson[i] = Base64::Encode( (unsigned char*)chunk.mVoxels, VoxelChunk::sSize * VoxelChunk::sSize * VoxelChunk::sSize );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Load( EcsSingleton& _component, const Json& _json )
    {
        VoxelTerrain& terrain = static_cast<VoxelTerrain&>( _component );
        Serializable::LoadInt3( _json, "size", terrain.mSize );
        Serializable::LoadInt( _json, "seed", terrain.mGenerator.mSeed );
        Serializable::LoadFixed( _json, "threshold", terrain.mGenerator.mThreshold );
        Serializable::LoadBool( _json, "clear_sides", terrain.mGenerator.mClearSides );
        NoiseOctave::Load( _json, "3d_octave_0", terrain.mGenerator.m3DOctaves[0] );
        NoiseOctave::Load( _json, "3d_octave_1", terrain.mGenerator.m3DOctaves[1] );
        NoiseOctave::Load( _json, "2d_octave", terrain.mGenerator.m2DOctave );

        fanAssert(terrain.mChunks == nullptr);
        terrain.mChunks = new VoxelChunk[terrain.mSize.x * terrain.mSize.y * terrain.mSize.z] {};

        const Json* chunksJson = Serializable::FindToken(_json, "chunks");

        if( chunksJson != nullptr )
        {
            for( int i = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; ++i )
            {
                VoxelChunk& chunk = terrain.mChunks[i];
                std::string data = Base64::Decode( (*chunksJson)[i] );
                fanAssert(data.length() == VoxelChunk::sSize * VoxelChunk::sSize * VoxelChunk::sSize);
                memcpy( chunk.mVoxels,data.data(), data.length() );
                chunk.mIsGenerated = true;
            }
        }
    }
}