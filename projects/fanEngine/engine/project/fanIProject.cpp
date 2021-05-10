#include "engine/project/fanIProject.hpp"

// base
#include "engine/fanSceneTags.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanExpirationTime.hpp"
#include "engine/components/fanFollowTransform.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanSceneResources.hpp"
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
#include "engine/singletons/fanRenderResources.hpp"
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
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void IProject::EcsIncludeEngine( EcsWorld& _world )
    {
        _world.AddComponentType<SceneNode>();
        _world.AddComponentType<Camera>();
        _world.AddComponentType<ExpirationTime>();
        _world.AddComponentType<FollowTransform>();
        _world.AddComponentType<Bounds>();

        _world.AddComponentType<Transform>();
        _world.AddComponentType<Scale>();

        _world.AddSingletonType<Scene>();
        _world.AddSingletonType<SceneResources>();
        _world.AddSingletonType<ScenePointers>();
        _world.AddSingletonType<Mouse>();
        _world.AddSingletonType<Application>();
        _world.AddSingletonType<Time>();
        _world.AddSingletonType<VoxelTerrain>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void IProject::EcsIncludePhysics( EcsWorld& _world )
    {
        _world.AddSingletonType<PhysicsWorld>();
        _world.AddComponentType<Rigidbody>();
        _world.AddComponentType<SphereCollider>();
        _world.AddComponentType<BoxCollider>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void IProject::EcsIncludeRender3D( EcsWorld& _world )
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void IProject::EcsIncludeRenderUI( EcsWorld& _world )
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
    void IProject::EcsIncludeNetworkClient( EcsWorld& _world )
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
    void IProject::EcsIncludeNetworkServer( EcsWorld& _world )
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