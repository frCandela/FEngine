#include "engine/singletons/fanRenderWorld.hpp"

#include "editor/fanModals.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &RenderWorld::Save;
        _info.load = &RenderWorld::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::Save( const EcsSingleton& _singleton, Json& _json )
    {
        const RenderWorld& renderWorld = static_cast<const RenderWorld&>( _singleton );
        Serializable::SaveColor( _json, "filter_color", renderWorld.mFilterColor );
        Serializable::SaveColor( _json, "clear_color", renderWorld.mClearColor );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::Load( EcsSingleton& _singleton, const Json& _json )
    {
        RenderWorld& renderWorld = static_cast<RenderWorld&>( _singleton );
        Serializable::LoadColor( _json, "filter_color", renderWorld.mFilterColor );
        Serializable::LoadColor( _json, "clear_color", renderWorld.mClearColor );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        RenderWorld& renderWorld = static_cast<RenderWorld&>( _singleton );
        renderWorld.drawData.clear();
        renderWorld.uiDrawData.clear();
        renderWorld.pointLights.clear();
        renderWorld.directionalLights.clear();
        renderWorld.mTargetSize                       = { 1920, 1080 };
        renderWorld.mParticlesMesh                    = new Mesh();
        renderWorld.mParticlesMesh->mSubMeshes.resize(1);
        renderWorld.mParticlesMesh->mSubMeshes[0].mHostVisible      = true;
        renderWorld.mParticlesMesh->mSubMeshes[0].mOptimizeVertices = false;
        renderWorld.mParticlesMesh->mAutoUpdateHull   = false;
        renderWorld.mIsHeadless                       = false;
        renderWorld.mClearColor                       = Color::sDarkGrey;
        renderWorld.mFullscreen                       = FullScreen {};
    }
}