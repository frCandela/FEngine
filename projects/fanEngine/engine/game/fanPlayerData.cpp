#include <engine/physics/fanTransform.hpp>
#include "fanPlayerData.hpp"
#include "core/time/fanProfiler.hpp"
#include "engine/game/fanIGame.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/resources/fanCursor.hpp"

// base
#include "engine/fanSceneTags.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanExpirationTime.hpp"
#include "engine/components/fanFollowTransform.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/singletons/fanScenePointers.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/terrain/fanVoxelTerrain.hpp"

// physics
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanScale.hpp"
#include "engine/physics/fanRigidbody.hpp"
#include "engine/physics/fanSphereCollider.hpp"
#include "engine/physics/fanBoxCollider.hpp"
#include "engine/physics/fanPhysicsWorld.hpp"

//render 3D
#include "engine/components/fanDirectionalLight.hpp"
#include "engine/components/fanPointLight.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanMaterial.hpp"
#include "engine/components/fanParticleEmitter.hpp"
#include "engine/components/fanParticle.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/singletons/fanApplication.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

// render ui
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIRenderer.hpp"
#include "engine/ui/fanUIProgressBar.hpp"
#include "engine/ui/fanUIButton.hpp"
#include "engine/ui/fanUILayout.hpp"
#include "engine/ui/fanUIAlign.hpp"
#include "engine/ui/fanUIText.hpp"

// network client
#include "network/components/fanReliabilityLayer.hpp"
#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "network/components/fanClientConnection.hpp"
#include "network/components/fanClientReplication.hpp"
#include "network/components/fanClientRPC.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanClientRollback.hpp"
#include "network/singletons/fanLinkingContext.hpp"

// network server
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanHostPersistentHandle.hpp"
#include "network/components/fanEntityReplication.hpp"
#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/singletons/fanSpawnManager.hpp"

namespace fan
{
    Mesh2D* CreateMesh2DQuad();

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    PlayerData::PlayerData( const LaunchSettings& _settings ) :
            mLaunchSettings( _settings ),
            mApplicationShouldExit( false ),
            mWindow( _settings.mWindowName, _settings.mWindow_position, _settings.mWindow_size, _settings.mIconPath ),
            mRenderer( mWindow, mResources, _settings.mLaunchEditor ? Renderer::ViewType::Editor : Renderer::ViewType::Game )
    {
        Mouse::SetCallbacks( mWindow.mWindow );

        mFullScreen.mWindowedPosition = mWindow.GetPosition();
        mFullScreen.mWindowedSize     = mWindow.GetSize();

        if( FT_Init_FreeType( &mFreetypeLib ) )
        {
            Debug::Error( "Could not init FreeType Library", Debug::Type::Engine );
        }

        fanAssert( ResourcePtrData::sResourceManager == nullptr );
        ResourcePtrData::sResourceManager = &mResources;

        {
            ResourceInfo info;
            info.mLoad = &LoadPrefab;
            mResources.AddResourceType<Prefab>( info );
        }

        {
            ResourceInfo info;
            info.mLoad    = &LoadFont;
            info.mDataPtr = &mFreetypeLib;
            mResources.AddResourceType<Font>( info );
        }

        {
            ResourceInfo info;
            info.mLoad           = &LoadTexture;
            info.mUseDirtyList   = true;
            info.mUseDestroyList = true;
            mResources.AddResourceType<Texture>( info );
        }

        {
            ResourceInfo info;
            info.mLoad           = &LoadMesh;
            info.mUseDirtyList   = true;
            info.mUseDestroyList = true;
            mResources.AddResourceType<Mesh>( info );
        }

        {
            ResourceInfo info;
            info.mUseDirtyList   = true;
            info.mUseDestroyList = true;
            mResources.AddResourceType<Mesh2D>( info );
        }

        {
            ResourceInfo info;
            mResources.AddResourceType<Cursor>( info );
        }

        mResources.Load<Texture>( RenderGlobal::sDefaultTexture );
        mResources.Load<Font>( RenderGlobal::sDefaultGameFont );
        mResources.Load<Texture>( RenderGlobal::sWhiteTexture );
        mResources.Load<Mesh>( RenderGlobal::sDefaultMesh );
        mResources.Add<Mesh2D>( CreateMesh2DQuad(), RenderGlobal::sMesh2DQuad );

        mRenderer.Create();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::Destroy()
    {
        mResources.Clear();
        mRenderer.Destroy();
        FT_Done_FreeType( mFreetypeLib );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size )
    {
        EcsWorld& world = _game.GetWorld();

        SCOPED_PROFILE( update_RW );
        RenderWorld      & renderWorld = world.GetSingleton<RenderWorld>();
        const RenderDebug& renderDebug = world.GetSingleton<RenderDebug>();
        renderWorld.mTargetSize = _size;

        // particles mesh
        if( !renderWorld.mParticlesMesh->mSubMeshes[0].mIndices.empty() )
        {
            RenderDataModel particlesDrawData;
            particlesDrawData.mMesh         = &renderWorld.mParticlesMesh->mSubMeshes[0];
            particlesDrawData.mModelMatrix  = glm::mat4( 1.f );
            particlesDrawData.mNormalMatrix = glm::mat4( 1.f );
            particlesDrawData.mColor        = glm::vec4( 1.f, 1.f, 1.f, 1.f );
            particlesDrawData.mShininess    = 1;
            particlesDrawData.mTexture      = nullptr;
            renderWorld.drawData.push_back( particlesDrawData );
        }

        {
            SCOPED_PROFILE( set_render_data );
            _renderer.SetDrawData( renderWorld.drawData );
            _renderer.SetUIDrawData( renderWorld.uiDrawData );
            _renderer.SetPointLights( renderWorld.pointLights );
            _renderer.SetDirectionalLights( renderWorld.directionalLights );
            _renderer.SetDebugDrawData( renderDebug.mDebugLines, renderDebug.mDebugLinesNoDepthTest, renderDebug.mDebugTriangles, renderDebug.mDebugTrianglesNoDepthTest, renderDebug.mDebugLines2D );
        }

        // Camera
        {
            SCOPED_PROFILE( update_camera );
            Scene& scene = world.GetSingleton<Scene>();
            EcsEntity cameraID = world.GetEntity( scene.mMainCameraHandle );
            Camera& camera = world.GetComponent<Camera>( cameraID );
            camera.mAspectRatio = Fixed::FromFloat( _size[0] / _size[1] );

            Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
            _renderer.SetMainCamera( camera.GetProjection(), camera.GetView( cameraTransform ), cameraTransform.mPosition.ToGlm() );
        }

        _renderer.mClearColor                       = renderWorld.mClearColor.ToGLM();
        _renderer.mDrawPostprocess.mUniforms.mColor = renderWorld.mFilterColor.ToGLM();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::MatchFullscreenState( const FullScreen& _fullscreen, Window& _window )
    {
        if( _window.IsFullscreen() != _fullscreen.mIsFullScreen )
        {
            if( _fullscreen.mIsFullScreen )
            {
                _window.SetFullscreen();
            }
            else
            {
                _window.SetWindowed( _fullscreen.mWindowedPosition, _fullscreen.mWindowedSize );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::MatchCursor( const Cursor* _cursor, Window& _window )
    {
        _window.SetCursor( _cursor != nullptr ? *_cursor : _window.mArrowCursor );
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
    void PlayerData::EcsIncludeEngine( EcsWorld& _world )
    {
        _world.AddComponentType<SceneNode>();
        _world.AddComponentType<Camera>();
        _world.AddComponentType<ExpirationTime>();
        _world.AddComponentType<FollowTransform>();
        _world.AddComponentType<Bounds>();

        _world.AddComponentType<Transform>();
        _world.AddComponentType<Scale>();

        _world.AddSingletonType<Scene>();
        _world.AddSingletonType<ScenePointers>();
        _world.AddSingletonType<Mouse>();
        _world.AddSingletonType<Application>();
        _world.AddSingletonType<Time>();
        _world.AddSingletonType<VoxelTerrain>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::EcsIncludePhysics( EcsWorld& _world )
    {
        _world.AddSingletonType<PhysicsWorld>();
        _world.AddComponentType<Rigidbody>();
        _world.AddComponentType<SphereCollider>();
        _world.AddComponentType<BoxCollider>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::EcsIncludeRender3D( EcsWorld& _world )
    {
        _world.AddComponentType<DirectionalLight>();
        _world.AddComponentType<PointLight>();
        _world.AddComponentType<MeshRenderer>();
        _world.AddComponentType<Material>();
        _world.AddComponentType<ParticleEmitter>();
        _world.AddComponentType<Particle>();

        _world.AddSingletonType<RenderWorld>();
        _world.AddSingletonType<RenderDebug>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::EcsIncludeRenderUI( EcsWorld& _world )
    {
        _world.AddComponentType<UITransform>();
        _world.AddComponentType<UIRenderer>();
        _world.AddComponentType<UIProgressBar>();
        _world.AddComponentType<UIButton>();
        _world.AddComponentType<UILayout>();
        _world.AddComponentType<UIAlign>();
        _world.AddComponentType<UIText>();

        _world.AddTagType<TagUIModified>();
        _world.AddTagType<TagUIEnabled>();
        _world.AddTagType<TagUIVisible>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::EcsIncludeNetworkClient( EcsWorld& _world )
    {
        _world.AddComponentType<ReliabilityLayer>();
        _world.AddComponentType<LinkingContextUnregisterer>();
        _world.AddComponentType<ClientConnection>();
        _world.AddComponentType<ClientReplication>();
        _world.AddComponentType<ClientRPC>();
        _world.AddComponentType<ClientGameData>();
        _world.AddComponentType<ClientRollback>();

        _world.AddSingletonType<LinkingContext>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PlayerData::EcsIncludeNetworkServer( EcsWorld& _world )
    {
        _world.AddComponentType<ReliabilityLayer>();
        _world.AddComponentType<LinkingContextUnregisterer>();
        _world.AddComponentType<HostGameData>();
        _world.AddComponentType<HostConnection>();
        _world.AddComponentType<HostReplication>();
        _world.AddComponentType<HostPersistentHandle>();
        _world.AddComponentType<EntityReplication>();

        _world.AddSingletonType<ServerConnection>();
        _world.AddSingletonType<LinkingContext>();
        _world.AddSingletonType<HostManager>();
        _world.AddSingletonType<SpawnManager>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* PlayerData::LoadPrefab( const std::string& _path, ResourceInfo& )
    {
        Debug::Log() << Debug::Type::Resources << "Loading prefab" << _path << Debug::Endl();
        Prefab* prefab = new Prefab();
        if( !prefab->CreateFromFile( _path ) )
        {
            delete prefab;
            return nullptr;
        }
        return prefab;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* PlayerData::LoadFont( const std::string& _path, ResourceInfo& _info )
    {
        Debug::Log() << Debug::Type::Resources << "Loading font " << _path << Debug::Endl();
        FT_Library& freetypeLib = *static_cast<FT_Library*>(_info.mDataPtr);
        Font      * font        = new Font( freetypeLib, _path );
        if( !font->IsValid() )
        {
            delete font;
            return nullptr;
        }
        return font;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* PlayerData::LoadTexture( const std::string& _path, ResourceInfo& )
    {
        Debug::Log() << Debug::Type::Resources << "Loading texture " << _path << Debug::Endl();
        Texture* texture = new Texture();
        if( !texture->LoadFromFile( _path ) )
        {
            delete texture;
            return nullptr;
        }
        return texture;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Resource* PlayerData::LoadMesh( const std::string& _path, ResourceInfo& )
    {
        Debug::Log() << Debug::Type::Resources << "Loading mesh " << _path << Debug::Endl();
        Mesh* mesh = new Mesh();
        if( !mesh->LoadFromFile( _path ) )
        {
            delete mesh;
            return nullptr;
        }
        return mesh;
    }
}