#include "game/fanProjectVoxels.hpp"

#include "core/math/fanFixedPoint.hpp"
#include "core/math/fanQuaternion.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/physics/fanUpdateFxRigidbodies.hpp"
#include "engine/systems/fanUpdateBounds.hpp"
#include "engine/systems/fanUpdateUIText.hpp"
#include "engine/systems/fanRaycastUI.hpp"
#include "engine/systems/fanUpdateUILayouts.hpp"
#include "engine/systems/fanUpdateUIAlign.hpp"
#include "engine/systems/fanUpdateTransforms.hpp"
#include "engine/systems/fanSynchronizeMotionStates.hpp"
#include "engine/singletons/fanPhysicsWorld.hpp"
#include "engine/systems/fanUpdateTimers.hpp"
#include "engine/systems/fanUpdateParticles.hpp"
#include "engine/systems/fanEmitParticles.hpp"
#include "engine/systems/fanGenerateParticles.hpp"
#include "engine/systems/fanRegisterPhysics.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"

#include "game/components/fanTestComponent.hpp"
#include "game/singletons/fanTestSingleton.hpp"
#include "game/systems/fanTestSystem.hpp"

#include "editor/fanRegisterEditorGui.hpp"


#include "engine/singletons/fanScene.hpp"
#include "engine/singletons/fanRenderResources.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    ProjectVoxels::ProjectVoxels()
    {
        mName = "project_voxels";
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::Init()
    {
        EcsIncludeEngine( mWorld );
        EcsIncludePhysics( mWorld );
        EcsIncludeRender3D( mWorld );
        EcsIncludeRenderUI( mWorld );

        mWorld.AddComponentType<TestComponent>();
        mWorld.AddSingletonType<TestSingleton>();

#ifdef FAN_EDITOR
        RegisterEditorGuiInfos( mWorld.GetSingleton<EditorGuiInfo>() );
#endif
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::Start()
    {
        mWorld.Run<SRegisterAllRigidbodies>();
        MeshManager& meshManager = *mWorld.GetSingleton<RenderResources>().mMeshManager;
        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        meshManager.Add( renderWorld.mParticlesMesh, "particles_mesh_" + mName );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::Stop()
    {
    }

    //============================================================================================================================
    //============================================================================================================================
    void ProjectVoxels::Step( const float _delta )
    {
        SCOPED_PROFILE( step );

        const Fixed fxDelta = Fixed::FromFloat( _delta );

        // physics & transforms
        PhysicsWorld& physicsWorld = mWorld.GetSingleton<PhysicsWorld>();
        mWorld.Run<SSynchronizeMotionStateFromTransform>();
        physicsWorld.mDynamicsWorld->stepSimulation( _delta, 10, Time::sPhysicsDelta );

        {
            FxPhysicsWorld& fxPhysicsWorld = mWorld.GetSingleton<FxPhysicsWorld>();


            mWorld.Run<SIntegrateFxRigidbodies>( fxDelta, fxPhysicsWorld );
            mWorld.Run<STestSystem>( fxDelta );
        }

        mWorld.Run<SSynchronizeTransformFromMotionState>();
        mWorld.Run<SMoveFollowTransforms>();

        // ui
        mWorld.Run<SUpdateUIText>();
        mWorld.Run<SAlignUI>();
        mWorld.Run<SUpdateUILayouts>();
        mWorld.Run<SHoverButtons>();
        mWorld.Run<SHighlightButtons>();

        // gameplay
        mWorld.Run<SUpdateExpirationTimes>( _delta );

        mWorld.Run<SUpdateParticles>( _delta );
        mWorld.Run<SEmitParticles>( _delta );
        mWorld.Run<SGenerateParticles>( _delta );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );

        RenderWorld& renderWorld = mWorld.GetSingleton<RenderWorld>();
        renderWorld.drawData.clear();

        mWorld.Run<SUpdateRenderWorldModelsFixed>( renderWorld );
        mWorld.Run<SUpdateRenderWorldModels>( renderWorld );
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }

    btVector3 Meh( Vector3 _v )
    {
        return btVector3( _v.x.ToFloat(), _v.y.ToFloat(), _v.z.ToFloat() );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    void ProjectVoxels::OnGui()
    {
        if( ImGui::Begin( "testoss" ) )
        {


            static glm::vec3 force = { 1, 0, 0 };
            static glm::vec3 point = { 0, 1, 0 };
            ImGui::DragFloat3( "force", &force.x );
            ImGui::DragFloat3( "point", &point.x );

            if( ImGui::CollapsingHeader("fixed"))
            {
                static EcsHandle handle = 0;
                if( ImGui::Button( "instantiate" ) )
                {
                    Scene    & scene = mWorld.GetSingleton<Scene>();
                    SceneNode& node  = scene.CreateSceneNode( "ball", &scene.GetRootNode() );
                    handle = node.mHandle;
                    RenderResources& resources = mWorld.GetSingleton<RenderResources>();

                    EcsEntity entity = mWorld.GetEntity( handle );
                    FxTransform& transform = mWorld.AddComponent<FxTransform>( entity );
                    transform.mPosition = Vector3::sUp;
                    mWorld.AddComponent<FxRigidbody>( entity );
                    Material& mat = mWorld.AddComponent<Material>( entity );
                    mat.mTexture = resources.mTextureManager->GetOrLoad( "_default/texture/uv_checker.png" );

                    MeshRenderer& meshRenderer = mWorld.AddComponent<MeshRenderer>( entity );
                    meshRenderer.mMesh = resources.mMeshManager->GetOrLoad( RenderGlobal::sMeshSphere );
                }

                ImGui::SameLine();
                if( ImGui::Button( "impulse" ) )
                {
                    EcsEntity entity = mWorld.GetEntity( handle );
                    FxRigidbody& rb = mWorld.GetComponent<FxRigidbody>( entity );

                    rb.ApplyForce( Math::ToFixed( force ), Math::ToFixed( point ) );
                }

                ImGui::SameLine();
                if( ImGui::Button( "reset" ) )
                {
                    EcsEntity entity = mWorld.GetEntity( handle );
                    FxRigidbody& rb = mWorld.GetComponent<FxRigidbody>( entity );
                    rb.mRotation = rb.mVelocity = Vector3::sZero;
                    FxTransform& transform = mWorld.GetComponent<FxTransform>( entity );
                    transform.mPosition = FIXED( 0.5 ) * Vector3::sUp;
                    transform.mRotation = Quaternion::sIdentity;

                    Debug::Log() <<"a"<< sizeof(FxRigidbody) << Debug::Endl();
                    Debug::Log() <<"b"<< sizeof(btRigidBody) << Debug::Endl();
                }
            }
            static bool registerLater = false;
            if( registerLater )
            {
                registerLater = false;
                mWorld.Run<SUnregisterAllRigidbodies>();
                mWorld.Run<SRegisterAllRigidbodies>();
            }

            if( ImGui::CollapsingHeader("bullet"))
            {
                static EcsHandle handle = 0;
                ImGui::PushID("bullet");
                if( ImGui::Button( "instantiate" ) )
                {

                    Scene    & scene = mWorld.GetSingleton<Scene>();
                    SceneNode& node  = scene.CreateSceneNode( "ball_bullet", &scene.GetRootNode() );
                    handle = node.mHandle;
                    RenderResources& resources = mWorld.GetSingleton<RenderResources>();

                    EcsEntity entity = mWorld.GetEntity( handle );
                    Transform& transform = mWorld.AddComponent<Transform>( entity );
                    transform.SetPosition( btVector3_Up );
                    Rigidbody& rigidbody = mWorld.AddComponent<Rigidbody>( entity );
                    rigidbody.EnableDeactivation(false);
                    SphereShape& shape = mWorld.AddComponent<SphereShape>( entity );
                    shape.SetRadius( 0.5f );
                    mWorld.AddComponent<MotionState>( entity );
                    Material& mat = mWorld.AddComponent<Material>( entity );
                    mat.mTexture = resources.mTextureManager->GetOrLoad( "_default/texture/uv_checker.png" );

                    MeshRenderer& meshRenderer = mWorld.AddComponent<MeshRenderer>( entity );
                    meshRenderer.mMesh = resources.mMeshManager->GetOrLoad( RenderGlobal::sMeshSphere );

                    registerLater = true;
                }


                ImGui::SameLine();
                if( ImGui::Button( "impulse" ) )
                {
                    EcsEntity entity = mWorld.GetEntity( handle );
                    Rigidbody& rb = mWorld.GetComponent<Rigidbody>( entity );

                    rb.mRigidbody->applyForce( ToBullet(force) ,  ToBullet(point)  );
                }

                ImGui::SameLine();
                if( ImGui::Button( "reset" ) )
                {
                    EcsEntity entity = mWorld.GetEntity( handle );
                    Rigidbody& rb = mWorld.GetComponent<Rigidbody>( entity );
                    rb.mRigidbody->setAngularVelocity( btVector3_Zero);
                    rb.mRigidbody->setLinearVelocity( btVector3_Zero);
                    Transform& transform = mWorld.GetComponent<Transform>( entity );
                    transform.SetPosition( 0.5*btVector3_Up );
                    transform.SetRotationEuler(btVector3_Zero);
                }
                ImGui::PopID();
            }

            ImGui::End();
        }
    }
}