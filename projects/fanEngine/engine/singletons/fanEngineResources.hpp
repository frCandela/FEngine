#pragma once

#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
    class PrefabManager;
    class MeshManager;
    class Mesh2DManager;
    class TextureManager;
    class FontManager;
    struct Mesh2D;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct EngineResources : public EcsSingleton
    {
    ECS_SINGLETON( EngineResources )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        static void SetupResources( PrefabManager& _prefabManager, MeshManager& _meshManager, Mesh2DManager& _mesh2DManager, TextureManager& _textureManager, FontManager& _font );
        void SetPointers( PrefabManager* _prefabManager, MeshManager* _meshManager, Mesh2DManager* _mesh2DManager, TextureManager* _textureManager, FontManager* _fontManager );
        static Mesh2D* CreateMesh2DQuad();

        PrefabManager * mPrefabManager  = nullptr;
        MeshManager   * mMeshManager    = nullptr;
        Mesh2DManager * mMesh2DManager  = nullptr;
        TextureManager* mTextureManager = nullptr;
        FontManager   * mFontManager    = nullptr;

        //std::vector<Cursor> mCursors;
    };
}