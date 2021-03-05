#include "fanIProjectContainer.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/math/fanMathUtils.hpp"
#include "engine/project/fanIProject.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/singletons/fanRenderResources.hpp"
#include "engine/singletons/fanSceneResources.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    IProjectContainer::IProjectContainer( const LaunchSettings& _settings ) :
            mLaunchSettings( _settings ),
            mWindow( _settings.mWindowName, _settings.mWindow_position, _settings.mWindow_size ),
            mRenderer( mWindow,
                       _settings.mLaunchEditor ? Renderer::ViewType::Editor : Renderer::ViewType::Game ),
            mApplicationShouldExit( false )
    {
        Mouse::SetCallbacks( mWindow.mWindow );
        mFullScreen.SavePreviousPositionAndSize( mWindow );

        SceneResources::SetupResources( mPrefabManager );
        RenderResources::SetupResources( mRenderer.mMeshManager,
                                         mRenderer.mMesh2DManager,
                                         mRenderer.mTextureManager,
                                         mRenderer.mFontManager );
        mWindow.SetIcon( _settings.mIconPath );
    }

    //========================================================================================================
    //========================================================================================================
    void IProjectContainer::Exit()
    {
        mApplicationShouldExit = true;
    }

    //========================================================================================================
    //========================================================================================================
    void IProjectContainer::InitWorld( EcsWorld& _world )
    {
        RenderResources& renderResources = _world.GetSingleton<RenderResources>();
        renderResources.SetPointers(&mRenderer.mMeshManager,
                                    &mRenderer.mMesh2DManager,
                                    &mRenderer.mTextureManager,
                                    &mRenderer.mFontManager );
        SceneResources& sceneResources = _world.GetSingleton<SceneResources>();
        sceneResources.SetPointers( &mPrefabManager );
    }

    //========================================================================================================
    //========================================================================================================
    void IProjectContainer::UpdateRenderWorld( Renderer& _renderer, IProject& _project, const glm::vec2 _size )
    {
        EcsWorld& world = _project.mWorld;

        SCOPED_PROFILE( update_RW );
        RenderWorld      & renderWorld = world.GetSingleton<RenderWorld>();
        const RenderDebug& renderDebug = world.GetSingleton<RenderDebug>();
        renderWorld.mTargetSize = _size;

        _project.UpdateRenderWorld();

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