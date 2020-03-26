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
 		_scene.onDeleteSceneNode.Connect( &EditorSelection::OnSceneNodeDeleted, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::DeleteSelection()
	{
		if ( m_selectedSceneNode != nullptr &&  ! m_selectedSceneNode->IsRoot() )
		{
			EcsWorld& world = *m_selectedSceneNode->scene->world;
			world.KillEntity( world.GetEntityID( m_selectedSceneNode->handle ) );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Update( const bool _gameWindowHovered )
	{
		SCOPED_PROFILE( selection );

		bool mouseCaptured = false;		

		// translation gizmo on selected scene node
		if ( m_selectedSceneNode != nullptr && m_selectedSceneNode != m_currentScene->mainCamera )
		{
			EcsWorld& world = *m_selectedSceneNode->scene->world;
			EntityID entityID = world.GetEntityID( m_selectedSceneNode->handle );
			if( world.HasComponent<Transform>( entityID ) )
			{
				Transform& transform = world.GetComponent< Transform >( entityID );
				btVector3 newPosition;
				if( EditorGizmos::Get().DrawMoveGizmo( btTransform( btQuaternion( 0, 0, 0 ), transform.GetPosition() ), (size_t)&transform, newPosition ) )
				{
					transform.SetPosition( newPosition );
					world.AddTag<tag_boundsOutdated>( entityID );
					mouseCaptured = true;
				}
			}
		}

		// mouse selection
 		if ( !mouseCaptured && _gameWindowHovered && Mouse::Get().GetButtonPressed( Mouse::button0 ) )
 		{
			EcsWorld& world = *m_currentScene->world;
			EntityID cameraID = world.GetEntityID( m_currentScene->mainCamera->handle );
			const Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
			const Camera& camera = world.GetComponent<Camera>( cameraID );

 			const Ray ray = camera.ScreenPosToRay( cameraTransform, Mouse::Get().GetScreenSpacePosition() );

			// raycast on bounds
			const Signature signatureRaycast = S_RaycastAll::GetSignature( world );
			std::vector<EntityID> outResults;
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

		// draw collision shapes, lights
		if( m_selectedSceneNode != nullptr )
		{
			EcsWorld& world = *m_selectedSceneNode->scene->world;
			EntityID nodeID = world.GetEntityID( m_selectedSceneNode->handle );
			S_DrawDebugCollisionShapes::Run( world, world.MatchSubset( S_DrawDebugCollisionShapes::GetSignature( world ), { nodeID } ) );
			S_DrawDebugDirectionalLights::Run( world, world.MatchSubset( S_DrawDebugDirectionalLights::GetSignature( world ), { nodeID } ) );
			S_DrawDebugPointLights::Run( world, world.MatchSubset( S_DrawDebugPointLights::GetSignature( world ), { nodeID } ) );
		}	

	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::SetSelectedSceneNode( SceneNode* _node )
	{
		m_selectedSceneNode = _node;
		onSceneNodeSelected.Emmit( m_selectedSceneNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Deselect()
	{
		m_selectedSceneNode = nullptr;
		onSceneNodeSelected.Emmit( m_selectedSceneNode );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::OnSceneNodeDeleted( SceneNode* _node )
	{
		if ( _node == m_selectedSceneNode )
		{
			Deselect();
		}
	}
}