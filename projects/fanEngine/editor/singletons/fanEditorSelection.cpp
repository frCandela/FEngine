#include "editor/singletons/fanEditorSelection.hpp"

#include "editor/singletons/fanEditorGizmos.hpp"
#include "platform/input/fanInput.hpp"
#include "platform/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/shapes/fanRay.hpp"
#include "core/fanDebug.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/components/fanPointLight.hpp"
#include "engine/components/fanDirectionalLight.hpp"
#include "engine/components/fanFollowTransform.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/systems/fanRaycast.hpp"
#include "engine/systems/fanDrawDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::Init( EcsWorld& _world, EcsSingleton& _component )
    {
        EditorSelection& editorSelection = static_cast<EditorSelection&>( _component );
        editorSelection.mCurrentScene = &_world.GetSingleton<Scene>();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::ConnectCallbacks( Scene& _scene )
    {
        _scene.mOnDeleteSceneNode.Connect( &EditorSelection::OnSceneNodeDeleted, this );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SceneNode* EditorSelection::GetSelectedSceneNode() const
    {
        if( mSelectedNodeHandle == 0 )
        {
            return nullptr;
        }
        else
        {
            EcsWorld& world = *mCurrentScene->mWorld;
            return &world.GetComponent<SceneNode>( world.GetEntity( mSelectedNodeHandle ) );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::DeleteSelection()
    {
        SceneNode* selectedSceneNode = GetSelectedSceneNode();
        if( selectedSceneNode != nullptr )
        {
            if( selectedSceneNode->HasFlag( SceneNode::NoDelete ) )
            {
                Debug::Warning() << "You cannot delete this node !" << Debug::Endl();
            }
            else
            {
                EcsWorld& world = *selectedSceneNode->mScene->mWorld;
                world.Kill( world.GetEntity( selectedSceneNode->mHandle ) );
                Deselect();
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool DrawMoveGizmo( SceneNode& _sceneNode )
    {
        EcsWorld& world = *_sceneNode.mScene->mWorld;
        EcsEntity entity = world.GetEntity( _sceneNode.mHandle );

        // Transform
        if( world.HasComponent<Transform>( entity ) )
        {
            Transform& transform = world.GetComponent<Transform>( entity );
            Vector3 newPosition;
            EditorGizmos& gizmos = world.GetSingleton<EditorGizmos>();
            if( gizmos.DrawMoveGizmo( Transform::Make( Quaternion::sIdentity, transform.mPosition ), SSID( "MoveGizmo" ), newPosition ) )
            {
                transform.mPosition = newPosition;
                _sceneNode.AddFlag( SceneNode::BoundsOutdated );
                return true;
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::Update( const bool _gameWindowHovered )
    {
        SCOPED_PROFILE( selection );

            bool mouseCaptured = false;

        // translation gizmo on selected scene node
        SceneNode* selectedSceneNode = GetSelectedSceneNode();
        if( selectedSceneNode != nullptr && selectedSceneNode->mHandle != mCurrentScene->mMainCameraHandle )
        {
            mouseCaptured = DrawMoveGizmo( *selectedSceneNode );
        }

        // mouse selection
        EcsWorld& world = *mCurrentScene->mWorld;
        Mouse   & mouse = world.GetSingleton<Mouse>();
        if( !mouseCaptured && _gameWindowHovered && mouse.mPressed[Mouse::buttonLeft] )
        {
            EcsEntity cameraID = world.GetEntity( mCurrentScene->mMainCameraHandle );
            const Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
            const Camera   & camera          = world.GetComponent<Camera>( cameraID );

            const Ray ray = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );

            // raycast on bounds
            std::vector<SRaycast::Result> results;
            world.Run<SRaycast>( ray, results );
            if( !results.empty() )
            {
                // cycle selection
                static Ray lastRay;
                static int cycle = 0;
                int        index = 0;
                if( ray == lastRay )
                {
                    index = ( ++cycle % results.size() );
                }
                lastRay          = ray;

                // selection
                SceneNode& sceneNode = world.GetComponent<SceneNode>( results[index].mEntity );
                SetSelectedSceneNode( &sceneNode );
            }
        }

        // draw collision shapes, lights
        if( mSelectedNodeHandle != 0 )
        {
            RenderDebug& renderDebug = world.GetSingleton<RenderDebug>();

            const EcsEntity selectedEntity = world.GetEntity( mSelectedNodeHandle );

            // bullet
            if( world.HasComponent<Transform>( selectedEntity ) )
            {
                const Transform& transform = world.GetComponent<Transform>( selectedEntity );

                //sphere collider
                if( world.HasComponent<SphereCollider>( selectedEntity ) )
                {
                    const SphereCollider& sphere = world.GetComponent<SphereCollider>( selectedEntity );
                    SDrawDebugFxSphereColliders::Draw( sphere, transform, renderDebug );
                }

                //sphere collider
                if( world.HasComponent<BoxCollider>( selectedEntity ) )
                {
                    const BoxCollider& box = world.GetComponent<BoxCollider>( selectedEntity );
                    SDrawDebugFxBoxColliders::Draw( box, transform, renderDebug );
                }
            }

            // fixed point
            if( world.HasComponent<Transform>( selectedEntity ) )
            {
                const Transform& transform = world.GetComponent<Transform>( selectedEntity );

                // Directional light
                if( world.HasComponent<DirectionalLight>( selectedEntity ) )
                {
                    const DirectionalLight& directionalLight = world.GetComponent<DirectionalLight>( selectedEntity );
                    SDrawDebugDirectionalLights::DrawDirectionalLight( renderDebug, transform, directionalLight );
                }

                // Point light
                if( world.HasComponent<PointLight>( selectedEntity ) )
                {
                    const PointLight& pointLight = world.GetComponent<PointLight>( selectedEntity );
                    SDrawDebugPointLights::DrawPointLight( renderDebug, transform, pointLight );
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::SetSelectedSceneNode( SceneNode* _node )
    {
        mSelectedNodeHandle = _node != nullptr ? _node->mHandle : 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::Deselect()
    {
        mSelectedNodeHandle = 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::OnSceneNodeDeleted( SceneNode* _node )
    {
        if( _node == GetSelectedSceneNode() )
        {
            Deselect();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSelection::OnToogleTransformLock()
    {
        SceneNode* selectedSceneNode = GetSelectedSceneNode();
        if( selectedSceneNode != nullptr )
        {
            EcsWorld& world = *selectedSceneNode->mScene->mWorld;
            EcsEntity entity = world.GetEntity( selectedSceneNode->mHandle );

            // FollowTransform
            if( world.HasComponent<FollowTransform>( entity ) )
            {
                FollowTransform& followTransform = world.GetComponent<FollowTransform>( entity );
                followTransform.mLocked = !followTransform.mLocked;

                Transform* transform = world.SafeGetComponent<Transform>( entity );
                SceneNode* sceneNode = world.SafeGetComponent<SceneNode>( entity );
                if( transform != nullptr && sceneNode != nullptr )
                {
                    FollowTransform::UpdateLocalTransform( followTransform, *transform, *sceneNode );
                }
            }
        }
    }
}