#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "engine/resources/fanCursors.hpp"

namespace fan
{
    class ResourceManager;
    class MeshManager;
    class Mesh2DManager;
    class TextureManager;
    struct Mesh2D;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct EngineResources : public EcsSingleton
    {
    ECS_SINGLETON( EngineResources )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        void SetupResources( ResourceManager& _resourceManager, MeshManager& _meshManager, Mesh2DManager& _mesh2DManager, TextureManager& _textureManager );

        ResourceManager * mResourceManager ;
        MeshManager   * mMeshManager   ;
        Mesh2DManager * mMesh2DManager ;
        TextureManager* mTextureManager;

        std::vector<Cursor> mCursors;
        Cursor* mCurrentCursor;
    };
}