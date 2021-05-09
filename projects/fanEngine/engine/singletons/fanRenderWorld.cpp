#include "engine/singletons/fanRenderWorld.hpp"

#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        RenderWorld& renderWorld = static_cast<RenderWorld&>( _component );
        renderWorld.drawData.clear();
        renderWorld.uiDrawData.clear();
        renderWorld.pointLights.clear();
        renderWorld.directionalLights.clear();
        renderWorld.mTargetSize                       = { 1920, 1080 };
        renderWorld.mParticlesMesh                    = new Mesh();
        renderWorld.mParticlesMesh->mHostVisible      = true;
        renderWorld.mParticlesMesh->mOptimizeVertices = false;
        renderWorld.mParticlesMesh->mAutoUpdateHull   = false;
        renderWorld.mIsHeadless                       = false;
    }
}