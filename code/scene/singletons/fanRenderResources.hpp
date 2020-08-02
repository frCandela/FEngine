#pragma once

#include <render/fanRenderGlobal.hpp>
#include "ecs/fanEcsSingleton.hpp"
#include "render/resources/fanMesh2D.hpp"

namespace fan
{
    struct Device;

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

        static void SetupResources( MeshManager& _meshManager,
                                    Mesh2DManager& _mesh2DManager,
                                    TextureManager& _textureManager );
        static Mesh2D* CreateMesh2DQuad();
        void SetPointers( MeshManager* _meshManager,
                          Mesh2DManager* _mesh2DManager,
                          TextureManager* _textureManager );

        MeshManager   * mMeshManager    = nullptr;
        Mesh2DManager * mMesh2DManager  = nullptr;
        TextureManager* mTextureManager = nullptr;
    };
}

