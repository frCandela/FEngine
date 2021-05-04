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
        _info.save = &VoxelTerrain::Save;
        _info.load = &VoxelTerrain::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _component );
        voxelTerrain.mTerrainHandle           = 0;
        voxelTerrain.mSize                    = { 4, 4, 4 };
        voxelTerrain.mChunks                  = nullptr;
        voxelTerrain.mIsInitialized           = false;
        voxelTerrain.mGenerator.mSeed         = 42;
        voxelTerrain.mGenerator.mSimplexNoise = SimplexNoise( voxelTerrain.mGenerator.mSeed );
        voxelTerrain.mGenerator.mAmplitude    = FIXED( 0.8 );
        voxelTerrain.mGenerator.mFrequency    = FIXED( 0.06 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::InitializeTerrain( EcsWorld& _world )
    {
        VoxelTerrain& terrain = _world.GetSingleton<VoxelTerrain>();
        fanAssert( terrain.mSize.x > 0 && terrain.mSize.y > 0 && terrain.mSize.z > 0 );

        terrain.mChunks = new VoxelChunk[terrain.mSize.x * terrain.mSize.y * terrain.mSize.z] {};
        terrain.mGenerator.Initialize();

        RenderResources& renderResources = _world.GetSingleton<RenderResources>();
        Texture        * texture         = renderResources.mTextureManager->GetOrLoad( "_default/texture/white.png" );

        Scene    & scene       = _world.GetSingleton<Scene>();
        SceneNode& terrainRoot = scene.CreateSceneNode( "terrain", &scene.GetRootNode() );
        terrain.mTerrainHandle = terrainRoot.mHandle;

        glm::ivec3 position;
        for( position.x        = 0; position.x < terrain.mSize.x; position.x++ )
        {
            for( position.y = 0; position.y < terrain.mSize.y; position.y++ )
            {
                for( position.z = 0; position.z < terrain.mSize.z; position.z++ )
                {
                    const std::string chunkName = "chunk_" + std::to_string( position.x ) + "_" + std::to_string( position.y ) + "_" + std::to_string( position.z );
                    SceneNode& chunkNode = scene.CreateSceneNode( chunkName, &terrainRoot );
                    EcsEntity entity = _world.GetEntity( chunkNode.mHandle );

                    VoxelChunk& chunk = terrain.GetChunk( position );
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
        terrain.mIsInitialized = true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::ClearTerrain( EcsWorld& _world )
    {
        VoxelTerrain   & terrain         = _world.GetSingleton<VoxelTerrain>();
        RenderResources& renderResources = _world.GetSingleton<RenderResources>();
        for( int i = 0; i < terrain.mSize.x * terrain.mSize.y * terrain.mSize.z; i++ )
        {
            VoxelChunk& chunk = terrain.mChunks[i];
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
        _world.Kill( _world.GetEntity( terrain.mTerrainHandle ) );
        delete[] terrain.mChunks;
        terrain.mChunks        = nullptr;
        terrain.mSize          = { 0, 0, 0 };
        terrain.mIsInitialized = false;
        terrain.mTerrainHandle = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Save( const EcsSingleton& _component, Json& _json )
    {
        const VoxelTerrain& voxelTerrain = static_cast<const VoxelTerrain&>( _component );
        Serializable::SaveInt( _json, "seed", voxelTerrain.mGenerator.mSeed );
        Serializable::SaveFixed( _json, "amplitude", voxelTerrain.mGenerator.mAmplitude );
        Serializable::SaveFixed( _json, "frequency", voxelTerrain.mGenerator.mFrequency );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void VoxelTerrain::Load( EcsSingleton& _component, const Json& _json )
    {
        VoxelTerrain& voxelTerrain = static_cast<VoxelTerrain&>( _component );
        Serializable::LoadInt( _json, "seed", voxelTerrain.mGenerator.mSeed );
        Serializable::LoadFixed( _json, "amplitude", voxelTerrain.mGenerator.mAmplitude );
        Serializable::LoadFixed( _json, "frequency", voxelTerrain.mGenerator.mFrequency );
    }
}