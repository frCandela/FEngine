#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector3.hpp"
#include "engine/terrain/fanVoxelChunk.hpp"
#include "engine/terrain/fanVoxelGeneration.hpp"

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
        static void PostInit( EcsWorld& _world, EcsSingleton& _singleton );
        static void Destroy( EcsWorld& _world, EcsSingleton& _singleton );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        static void InitializeTerrain( EcsWorld& _world, VoxelTerrain& _terrain );
        static void ClearTerrain( EcsWorld& _world, VoxelTerrain& _terrain );

        VoxelChunk& GetChunk( const glm::ivec3& _pos ) { return mChunks[_pos.x + mSize.x * _pos.y + mSize.x * mSize.y * _pos.z]; }
        const VoxelChunk& GetChunk( const glm::ivec3& _pos ) const { return mChunks[_pos.x + mSize.x * _pos.y + mSize.x * mSize.y * _pos.z]; }

        glm::ivec3 mSize;
        VoxelChunk* mChunks; // 3D array
        bool mIsInitialized = false;
        VoxelGenerator mGenerator;
        EcsHandle mTerrainHandle;
    };
}