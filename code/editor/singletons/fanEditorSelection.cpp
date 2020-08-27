#include "editor/singletons/fanEditorSelection.hpp"

#include "editor/singletons/fanEditorGizmos.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/shapes/fanRay.hpp"
#include "core/fanDebug.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/singletons/fanMouse.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/systems/fanRaycast.hpp"
#include "scene/systems/fanDrawDebug.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorSelection::SetInfo( EcsSingletonInfo& _info )
	{
		_info.name = "editor selection";
		_info.icon = ImGui::SELECTION16;
		_info.group = EngineGroups::Editor;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		EditorSelection& editorSelection = static_cast<EditorSelection&>( _component );
		editorSelection.mCurrentScene = &_world.GetSingleton<Scene>();
	}

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::ConnectCallbacks( Scene& _scene )
	{
		Input::Get().Manager().FindEvent( "delete" )->Connect( &EditorSelection::DeleteSelection, this );

        Signal<>& toogleLockEvent = * Input::Get().Manager().FindEvent( "toogle_follow_transform_lock" );
        toogleLockEvent.Connect( &EditorSelection::OnToogleTransformLock, this );

 		_scene.mOnDeleteSceneNode.Connect( &EditorSelection::OnSceneNodeDeleted, this );
	}

	//========================================================================================================
	//========================================================================================================
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

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::DeleteSelection()
	{
		SceneNode* selectedSceneNode = GetSelectedSceneNode();
		if ( selectedSceneNode != nullptr )
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

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::Update( const bool _gameWindowHovered )
	{
		SCOPED_PROFILE( selection );

		bool mouseCaptured = false;		

		// translation gizmo on selected scene node
		SceneNode* selectedSceneNode = GetSelectedSceneNode();
		if ( selectedSceneNode != nullptr && selectedSceneNode->mHandle != mCurrentScene->mMainCameraHandle )
		{
			EcsWorld& world = *selectedSceneNode->mScene->mWorld;
			EcsEntity entity = world.GetEntity( selectedSceneNode->mHandle );
			if( world.HasComponent<Transform>( entity ) )
			{
				Transform& transform = world.GetComponent< Transform >( entity );
				btVector3 newPosition;

				EditorGizmos& gizmos = world.GetSingleton<EditorGizmos>();
                if( gizmos.DrawMoveGizmo( btTransform( btQuaternion( 0, 0, 0 ), transform.GetPosition() ),
                                          (size_t)&transform,
                                          newPosition ) )
				{
					transform.SetPosition( newPosition );
					selectedSceneNode->AddFlag( SceneNode::BoundsOutdated );
					mouseCaptured = true;
				}
			}
		}

		// mouse selection
        EcsWorld& world = *mCurrentScene->mWorld;
        Mouse   & mouse = world.GetSingleton<Mouse>();
        if( !mouseCaptured && _gameWindowHovered && mouse.mPressed[ Mouse::buttonLeft ] )
 		{
			EcsEntity cameraID = world.GetEntity( mCurrentScene->mMainCameraHandle );
			const Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
			const Camera& camera = world.GetComponent<Camera>( cameraID );

            const Ray ray = camera.ScreenPosToRay( cameraTransform,
                                                   ToBullet( mouse.LocalScreenSpacePosition() ) );

			// raycast on bounds
			std::vector<EcsEntity> results;
			world.Run<S_RaycastAll>( ray, results );
			if( !results.empty() )
			{
				// cycle selection
				static Ray lastRay;
				static int cycle = 0;
				int index = 0;
				if( ray == lastRay )
				{					
					index = ( ++cycle % results.size() );
				}
				lastRay = ray;

				// selection
				SceneNode& sceneNode = world.GetComponent<SceneNode>( results[index] );
				SetSelectedSceneNode( &sceneNode );
			}
 		}

		// draw collision shapes, lights @migration
		if( mSelectedNodeHandle != 0 )
		{
			const EcsEntity selectedEntity = world.GetEntity( mSelectedNodeHandle );
			S_DrawDebugCollisionShapes::DrawCollisionShape( world, selectedEntity );

			if( world.HasComponent<Transform>( selectedEntity ) ) 
			{
				RenderDebug& renderDebug = world.GetSingleton<RenderDebug>();
				const Transform& transform = world.GetComponent<Transform>( selectedEntity );
				if( world.HasComponent<DirectionalLight>( selectedEntity ) )
                {
                    const DirectionalLight& directionalLight = world.GetComponent<DirectionalLight>(
                            selectedEntity );
                    S_DrawDebugDirectionalLights::DrawDirectionalLight( renderDebug,
                                                                        transform,
                                                                        directionalLight );
				} 
				if( world.HasComponent<PointLight>( selectedEntity ) )
				{
					const PointLight& pointLight = world.GetComponent<PointLight>( selectedEntity );
					S_DrawDebugPointLights::DrawPointLight( renderDebug, transform, pointLight );
				}
			}
		}	
	}

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::SetSelectedSceneNode( SceneNode* _node )
	{
		mSelectedNodeHandle = _node != nullptr ? _node->mHandle : 0;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::Deselect()
	{
		mSelectedNodeHandle = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorSelection::OnSceneNodeDeleted( SceneNode* _node )
	{
		if ( _node == GetSelectedSceneNode() )
		{
			Deselect();
		}
	}

	//========================================================================================================
	//========================================================================================================
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
				FollowTransform& follower = world.GetComponent<FollowTransform>( entity );
				follower.mLocked = !follower.mLocked;
				FollowTransform::UpdateLocalTransform( world, entity );
			}
		}
	}
}