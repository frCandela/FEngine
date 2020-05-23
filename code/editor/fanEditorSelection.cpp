#include "editor/fanEditorSelection.hpp"

#include "editor/fanEditorGizmos.hpp"
#include "editor/fanEditorDebug.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanMouse.hpp"
#include "core/math/shapes/fanRay.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/systems/fanRaycast.hpp"
#include "scene/systems/fanDrawDebug.hpp"
#include "scene/fanSceneTags.hpp"
#include "render/fanMesh.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================	
	EditorSelection::EditorSelection( Scene& _currentScene ) : m_currentScene( &_currentScene ) {}

	//================================================================================================================================
	//================================================================================================================================	
	void EditorSelection::ConnectCallbacks( Scene& _scene )
	{
		Input::Get().Manager().FindEvent( "delete" )->Connect( &EditorSelection::DeleteSelection, this );
		Input::Get().Manager().FindEvent( "toogle_follow_transform_lock" )->Connect( &EditorSelection::OnToogleTransformLock, this );
 		_scene.onDeleteSceneNode.Connect( &EditorSelection::OnSceneNodeDeleted, this );
	}

	//================================================================================================================================
	//================================================================================================================================	
	SceneNode* EditorSelection::GetSelectedSceneNode() const 
	{ 
		if( m_selectedNodeHandle == 0 )
		{
			return nullptr;
		}
		else
		{
			EcsWorld& world = *m_currentScene->world;
			return &world.GetComponent<SceneNode>( world.GetEntity( m_selectedNodeHandle ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::DeleteSelection()
	{
		SceneNode* selectedSceneNode = GetSelectedSceneNode();
		if ( selectedSceneNode != nullptr &&  !selectedSceneNode->IsRoot() )
		{
			EcsWorld& world = *selectedSceneNode->scene->world;
			world.Kill( world.GetEntity( selectedSceneNode->handle ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Update( const bool _gameWindowHovered )
	{
		SCOPED_PROFILE( selection );

		bool mouseCaptured = false;		

		// translation gizmo on selected scene node
		SceneNode* selectedSceneNode = GetSelectedSceneNode();
		if ( selectedSceneNode != nullptr && selectedSceneNode->handle != m_currentScene->mainCameraHandle )
		{
			EcsWorld& world = *selectedSceneNode->scene->world;
			EcsEntity entity = world.GetEntity( selectedSceneNode->handle );
			if( world.HasComponent<Transform>( entity ) )
			{
				Transform& transform = world.GetComponent< Transform >( entity );
				btVector3 newPosition;
				if( EditorGizmos::Get().DrawMoveGizmo( btTransform( btQuaternion( 0, 0, 0 ), transform.GetPosition() ), (size_t)&transform, newPosition ) )
				{
					transform.SetPosition( newPosition );
					world.AddTag<tag_boundsOutdated>( entity );
					mouseCaptured = true;
				}
			}
		}

		// mouse selection
 		if ( !mouseCaptured && _gameWindowHovered && Mouse::Get().GetButtonPressed( Mouse::button0 ) )
 		{
			EcsWorld& world = *m_currentScene->world;
			EcsEntity cameraID = world.GetEntity( m_currentScene->mainCameraHandle );
			const Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
			const Camera& camera = world.GetComponent<Camera>( cameraID );

 			const Ray ray = camera.ScreenPosToRay( cameraTransform, Mouse::Get().GetScreenSpacePosition() );

			// raycast on bounds
			const EcsSignature signatureRaycast = S_RaycastAll::GetSignature( world );
			std::vector<EcsEntity> outResults;
			if( S_RaycastAll::Run( world, world.Match( signatureRaycast ), ray, outResults ) )
			{
				// cycle selection
				static Ray lastRay;
				static int cycle = 0;
				int index = 0;
				if( ray == lastRay )
				{					
					index = ( ++cycle % outResults.size() );
				}
				lastRay = ray;

				// selection
				SceneNode& sceneNode = world.GetComponent<SceneNode>( outResults[index] );
				SetSelectedSceneNode( &sceneNode );
			}
			else
			{
				SetSelectedSceneNode( nullptr );
			}
 		}

		// draw collision shapes, lights @migration
// 		if( m_selectedSceneNode != nullptr )
// 		{
// 			EcsWorld& world = *m_selectedSceneNode->scene->world;
// 			EcsEntity nodeID = world.GetEntity( m_selectedSceneNode->handle );
// 			S_DrawDebugCollisionShapes::Run( world, world.MatchSubset( S_DrawDebugCollisionShapes::GetSignature( world ), { nodeID } ) );
// 			S_DrawDebugDirectionalLights::Run( world, world.MatchSubset( S_DrawDebugDirectionalLights::GetSignature( world ), { nodeID } ) );
// 			S_DrawDebugPointLights::Run( world, world.MatchSubset( S_DrawDebugPointLights::GetSignature( world ), { nodeID } ) );
// 		}	

	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::SetSelectedSceneNode( SceneNode* _node )
	{
		m_selectedNodeHandle = _node != nullptr ? _node->handle : 0;
		onSceneNodeSelected.Emmit( _node );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Deselect()
	{
		SceneNode* selectedSceneNode = GetSelectedSceneNode();
		m_selectedNodeHandle = 0;
		onSceneNodeSelected.Emmit( selectedSceneNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::OnSceneNodeDeleted( SceneNode* _node )
	{
		if ( _node == GetSelectedSceneNode() )
		{
			Deselect();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::OnToogleTransformLock()
	{
		SceneNode* selectedSceneNode = GetSelectedSceneNode();
		if( selectedSceneNode != nullptr )
		{
			EcsWorld& world = *selectedSceneNode->scene->world;
			EcsEntity entity = world.GetEntity( selectedSceneNode->handle );

			// FollowTransform
			if( world.HasComponent<FollowTransform>( entity ) )
			{
				FollowTransform& follower = world.GetComponent<FollowTransform>( entity );
				follower.locked = !follower.locked;
				FollowTransform::UpdateLocalTransform( world, entity );
			}

			// FollowTransformUI
			if( world.HasComponent<FollowTransformUI>( entity ) )
			{
				FollowTransformUI& follower = world.GetComponent<FollowTransformUI>( entity );
				follower.locked = !follower.locked;
				FollowTransformUI::UpdateOffset( world, entity );
			}
		}
		
	}
}