#include "engine/singletons/fanEngineResources.hpp"
#include "core/resources/fanResourceManager.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "render/resources/fanMesh2DManager.hpp"
#include "render/resources/fanMeshManager.hpp"
#include "engine/resources/fanFont.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EngineResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EngineResources::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        EngineResources& resources = static_cast<EngineResources&>( _singleton );
        resources.mResourceManager  = nullptr;
        resources.mMeshManager    = nullptr;
        resources.mMesh2DManager  = nullptr;
        resources.mCursors.clear();
        resources.mCurrentCursor = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Mesh2D* CreateMesh2DQuad()
    {
        Mesh2D* mesh2D = new Mesh2D();
        std::vector<UIVertex> vertices = {
                UIVertex( glm::vec2( +2.f, +0.f ), glm::vec2( +1.f, +0.f ) ),
                UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) ),
                UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) ),
                UIVertex( glm::vec2( +0.f, +0.f ), glm::vec2( +0.f, +0.f ) ),
                UIVertex( glm::vec2( +0.f, +2.f ), glm::vec2( +0.f, +1.f ) ),
                UIVertex( glm::vec2( +2.f, +2.f ), glm::vec2( +1.f, +1.f ) )
        };
        mesh2D->LoadFromVertices( vertices );
        return mesh2D;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EngineResources::SetupResources( ResourceManager& _resourceManager, MeshManager& _meshManager, Mesh2DManager& _mesh2DManager)
    {
        ResourcePtr<Prefab>::sOnResolve.Connect( &ResourceManager::ResolvePtr, &_resourceManager );
        ResourcePtr<Font>::sOnResolve.Connect( &ResourceManager::ResolvePtr, &_resourceManager );
        ResourcePtr<Texture>::sOnResolve.Connect( &ResourceManager::ResolvePtr, &_resourceManager );
        ResourcePtr<Mesh>::sOnResolve.Connect( &MeshManager::ResolvePtr, &_meshManager );

        _resourceManager.Load<Font>( RenderGlobal::sDefaultGameFont );
        _resourceManager.Load<Texture>( RenderGlobal::sWhiteTexture );

        _meshManager.Load( RenderGlobal::sDefaultMesh );
        Mesh2D* quad2D = CreateMesh2DQuad();
        _mesh2DManager.Add( quad2D, RenderGlobal::sMesh2DQuad );


        mResourceManager  = &_resourceManager;
        mMeshManager    = &_meshManager;
        mMesh2DManager  = &_mesh2DManager;
    }
}
