#pragma once

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
    struct Mesh2D;
    class MeshManager;
    class Mesh2DManager;
    class TextureManager;

    //========================================================================================================
    //========================================================================================================
    struct RenderResources : public EcsSingleton
    {
        ECS_SINGLETON( RenderResources )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );
        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );

        static Mesh2D* CreateMesh2DQuad();

        static void SetupResources( MeshManager& _meshManager,
                                    Mesh2DManager& _mesh2DManager,
                                    TextureManager& _textureManager );

        void SetPointers( MeshManager* _meshManager,
                          Mesh2DManager* _mesh2DManager,
                          TextureManager* _textureManager );

        MeshManager   * mMeshManager    = nullptr;
        Mesh2DManager * mMesh2DManager  = nullptr;
        TextureManager* mTextureManager = nullptr;
    };
}

