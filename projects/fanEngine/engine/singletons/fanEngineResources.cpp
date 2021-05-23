#include "engine/singletons/fanEngineResources.hpp"
#include "core/resources/fanResourceManager.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanMesh2D.hpp"
#include "engine/resources/fanFont.hpp"
#include "render/fanRenderGlobal.hpp"

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
        resources.mResources = nullptr;
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
        mesh2D->mPath = RenderGlobal::sMesh2DQuad;
        return mesh2D;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EngineResources::SetupResources( ResourceManager& _resourceManager )
    {
        _resourceManager.Load<Font>( RenderGlobal::sDefaultGameFont );
        _resourceManager.Load<Texture>( RenderGlobal::sWhiteTexture );
        _resourceManager.Load<Mesh>( RenderGlobal::sDefaultMesh );
        _resourceManager.Add<Mesh2D>( CreateMesh2DQuad(), RenderGlobal::sMesh2DQuad );

        mResources = &_resourceManager;
    }
}
