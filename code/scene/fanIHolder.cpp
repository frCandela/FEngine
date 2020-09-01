#include "scene/fanIHolder.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/math/fanMathUtils.hpp"
#include "render/fanRenderer.hpp"
#include "scene/fanIGame.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanTransform.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    IHolder::~IHolder()
    {
        mPrefabManager.Clear();
        delete mRenderer;
        mWindow.Destroy();
    }

    //========================================================================================================
    //========================================================================================================
    void IHolder::Exit()
    {
        mApplicationShouldExit = true;
    }

    //========================================================================================================
    //========================================================================================================
    void IHolder::UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size )
    {
        EcsWorld& world = _game.mWorld;

        SCOPED_PROFILE( update_RW );
        RenderWorld      & renderWorld = world.GetSingleton<RenderWorld>();
        const RenderDebug& renderDebug = world.GetSingleton<RenderDebug>();
        renderWorld.mTargetSize = _size;

        _game.UpdateRenderWorld();

        // particles mesh
        RenderDataModel particlesDrawData;
        particlesDrawData.mMesh         = renderWorld.mParticlesMesh;
        particlesDrawData.mModelMatrix  = glm::mat4( 1.f );
        particlesDrawData.mNormalMatrix = glm::mat4( 1.f );
        particlesDrawData.mColor        = glm::vec4( 1.f, 1.f, 1.f, 1.f );
        particlesDrawData.mShininess    = 1;
        particlesDrawData.mTextureIndex = 1;
        renderWorld.drawData.push_back( particlesDrawData );

        {
            SCOPED_PROFILE( set_render_data );
            _renderer.SetDrawData( renderWorld.drawData );
            _renderer.SetUIDrawData( renderWorld.uiDrawData );
            _renderer.SetPointLights( renderWorld.pointLights );
            _renderer.SetDirectionalLights( renderWorld.directionalLights );
            _renderer.SetDebugDrawData( renderDebug.mDebugLines,
                                        renderDebug.mDebugLinesNoDepthTest,
                                        renderDebug.mDebugTriangles,
                                        renderDebug.mDebugLines2D );
        }

        // Camera
        {
            SCOPED_PROFILE( update_camera );
            Scene& scene = world.GetSingleton<Scene>();
            EcsEntity cameraID = world.GetEntity( scene.mMainCameraHandle );
            Camera& camera = world.GetComponent<Camera>( cameraID );
            camera.mAspectRatio = _size[0] / _size[1];
            Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
            _renderer.SetMainCamera(
                    camera.GetProjection(),
                    camera.GetView( cameraTransform ),
                    ToGLM( cameraTransform.GetPosition() )
            );
        }
    }
}