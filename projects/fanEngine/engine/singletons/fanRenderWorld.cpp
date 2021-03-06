#include "engine/singletons/fanRenderWorld.hpp"

#include "editor/fanModals.hpp"
#include "core/memory/fanSerializable.hpp"
#include "core/resources/fanResources.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save     = &RenderWorld::Save;
        _info.load     = &RenderWorld::Load;
        _info.postInit = &RenderWorld::PostInit;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::Update( EcsWorld& _world )
    {
        RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();

        EcsView transparentsView = _world.Match( SUpdateRenderWorldModels::GetSignature( _world ) | _world.GetSignature<TagTransparent>(), EcsSignature( 0 ) );
        SUpdateRenderWorldModels::Run( _world, transparentsView, renderWorld.mTransparentModels );

        EcsView opaquesView = _world.Match( SUpdateRenderWorldModels::GetSignature( _world ), _world.GetSignature<TagTransparent>() );
        SUpdateRenderWorldModels::Run( _world, opaquesView, renderWorld.mOpaqueModels );

        _world.ForceRun<SUpdateRenderWorldModelsSkinned>( renderWorld );
        _world.ForceRun<SUpdateRenderWorldUI>( renderWorld );
        _world.ForceRun<SUpdateRenderWorldPointLights>( renderWorld );
        _world.ForceRun<SUpdateRenderWorldDirectionalLights>( renderWorld );
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
        renderWorld.mTransparentModels.clear();
        renderWorld.mOpaqueModels.clear();
        renderWorld.mSkinnedModels.clear();
        renderWorld.mUIModels.clear();
        renderWorld.mPointLights.clear();
        renderWorld.mDirectionalLights.clear();
        renderWorld.mTargetSize    = { 1920, 1080 };
        renderWorld.mIsHeadless    = false;
        renderWorld.mClearColor    = Color::sDarkGrey;
        renderWorld.mFullscreen    = FullScreen {};
        renderWorld.mParticlesMesh = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderWorld::PostInit( EcsWorld& _world, EcsSingleton& _singleton )
    {
        RenderWorld& renderWorld = static_cast<RenderWorld&>( _singleton );
        Resources  & resources   = *_world.GetSingleton<Application>().mResources;
        if( renderWorld.mParticlesMesh == nullptr )
        {
            renderWorld.mParticlesMesh = resources.Add( new Mesh, "particles_mesh" + std::to_string( resources.GetUniqueID() ) );
            renderWorld.mParticlesMesh->mSubMeshes.resize( 1 );
            renderWorld.mParticlesMesh->mSubMeshes[0].mHostVisible      = true;
            renderWorld.mParticlesMesh->mSubMeshes[0].mOptimizeVertices = false;
            renderWorld.mParticlesMesh->mAutoGenerateBoundingVolumes    = false;
            renderWorld.mParticlesMesh->mIsGenerated                    = true;
        }
    }
}