#include "core/time/fanScopedTimer.hpp"
#include "core/memory/fanSerializable.hpp"
#include "engine/singletons/fanRenderResources.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanFxTransform.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanMaterial.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"
#include "render/resources/fanTextureManager.hpp"

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
        if( _terrain.mSize.x <= 0 && _terrain.mSize.y <= 0 && _terrain.mSize.z <= 0 ){ return; }

        _terrain.mChunks = new VoxelChunk[_terrain.mSize.x * _terrain.mSize.y * _terrain.mSize.z] {};

        RenderResources& renderResources = _world.GetSingleton<RenderResources>();
        Texture        * texture         = renderResources.mTextureManager->GetOrLoad( "_default/texture/white.png" );

        Scene    & scene       = _world.GetSingleton<Scene>();
        SceneNode& terrainRoot = scene.CreateSceneNode( "terrain", &scene.GetRootNode() );
        terrainRoot.AddFlag( SceneNode::NoSave );
        _terrain.mTerrainHandle = terrainRoot.mHandle;

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

        RenderResources& renderResources = _world.GetSingleton<RenderResources>();
        for( int i = 0; i < _terrain.mSize.x * _terrain.mSize.y * _terrain.mSize.z; i++ )
        {
            VoxelChunk& chunk = _terrain.mChunks[i];
            if( chunk.mHandle != 0 && _world.HandleExists( chunk.mHandle ) )
            {
                EcsEntity entity = _world.GetEntity( chunk.mHandle );
                MeshRenderer& renderer = _world.AddComponent<MeshRenderer>( entity );
                Mesh        * mesh     = *renderer.mMesh;
                renderResources.mMeshManager->Remove( mesh->mPath );
                mesh->mPath = "";
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
        const VoxelTerrain& voxelTerrain = static_cast<const VoxelTerrain&>( _component );
        Serializable::SaveInt3( _json, "size", voxelTerrain.mSize );
        Serializable::SaveInt( _json, "seed", voxelTerrain.mGenerator.mSeed );
        Serializable::SaveFixed( _json, "threshold", voxelTerrain.mGenerator.mThreshold );
        Serializable::SaveBool( _json, "clear_sides", voxelTerrain.mGenerator.mClearSides );
        NoiseOctave::Save( _json, "3d_octave_0", voxelTerrain.mGenerator.m3DOctaves[0] );
        NoiseOctave::Save( _json, "3d_octave_1", voxelTerrain.mGenerator.m3DOctaves[1] );
        NoiseOctave::Save( _json, "2d_octave", voxelTerrain.mGenerator.m2DOctave );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Load( EcsSingleton& _component, const Json& _json )
    {
        VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _component );
        Serializable::LoadInt3( _json, "size", voxelTerrain.mSize );
        Serializable::LoadInt( _json, "seed", voxelTerrain.mGenerator.mSeed );
        Serializable::LoadFixed( _json, "threshold", voxelTerrain.mGenerator.mThreshold );
        Serializable::LoadBool( _json, "clear_sides", voxelTerrain.mGenerator.mClearSides );
        NoiseOctave::Load( _json, "3d_octave_0", voxelTerrain.mGenerator.m3DOctaves[0] );
        NoiseOctave::Load( _json, "3d_octave_1", voxelTerrain.mGenerator.m3DOctaves[1] );
        NoiseOctave::Load( _json, "2d_octave", voxelTerrain.mGenerator.m2DOctave );
    }
}