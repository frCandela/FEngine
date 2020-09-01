#include "scene/fanIGame.hpp"

// base
#include "scene/fanSceneTags.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanExpirationTime.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/singletons/fanSceneResources.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "scene/singletons/fanApplication.hpp"
#include "network/singletons/fanTime.hpp"

// physics
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"

//render 3D
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/singletons/fanRenderDebug.hpp"

// render ui
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/ui/fanUIProgressBar.hpp"
#include "scene/components/ui/fanUIButton.hpp"
#include "scene/components/ui/fanUILayout.hpp"
#include "scene/components/ui/fanUIAlign.hpp"
#include "scene/components/ui/fanUIText.hpp"

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
    //========================================================================================================
    //========================================================================================================
    void IGame::EcsIncludeBase    ( EcsWorld& _world )
    {
        _world.AddComponentType<SceneNode>();
        _world.AddComponentType<Transform>();
        _world.AddComponentType<Camera>();
        _world.AddComponentType<ExpirationTime>();
        _world.AddComponentType<FollowTransform>();

        _world.AddSingletonType<Scene>();
        _world.AddSingletonType<SceneResources>();
        _world.AddSingletonType<ScenePointers>();
        _world.AddSingletonType<Mouse>();
        _world.AddSingletonType<Application>();
        _world.AddSingletonType<Time>();
    }

    //========================================================================================================
    //========================================================================================================
    void IGame::EcsIncludePhysics ( EcsWorld& _world )
    {
        _world.AddComponentType<Rigidbody>();
        _world.AddComponentType<MotionState>();
        _world.AddComponentType<BoxShape>();
        _world.AddComponentType<SphereShape>();

        _world.AddSingletonType<PhysicsWorld>();
    }

    //========================================================================================================
    //========================================================================================================
    void IGame::EcsIncludeRender3D( EcsWorld& _world )
    {
        _world.AddComponentType<DirectionalLight>();
        _world.AddComponentType<PointLight>();
        _world.AddComponentType<MeshRenderer>();
        _world.AddComponentType<Material>();
        _world.AddComponentType<ParticleEmitter>();
        _world.AddComponentType<Particle>();

        _world.AddSingletonType<RenderWorld>();
        _world.AddSingletonType<RenderResources>();
        _world.AddSingletonType<RenderDebug>();
    }

    //========================================================================================================
    //========================================================================================================
    void IGame::EcsIncludeRenderUI( EcsWorld& _world )
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

    //========================================================================================================
    //========================================================================================================
    void IGame::EcsIncludeNetworkClient ( EcsWorld& _world )
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

    //========================================================================================================
    //========================================================================================================
    void IGame::EcsIncludeNetworkServer( EcsWorld& _world )
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
}