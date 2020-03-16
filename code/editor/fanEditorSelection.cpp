#include "editor/fanEditorSelection.hpp"
#include "editor/fanEditorGizmos.hpp"
#include "editor/fanEditorDebug.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanMouse.hpp"
#include "core/math/shapes/fanRay.hpp"
#include "scene/components/ui/fanUIMeshRenderer.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/components/fanCamera2.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanScene.hpp"
#include "render/fanMesh.hpp"
#include "scene/ecs/systems/fanRaycast.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================	
	EditorSelection::EditorSelection( Scene*& _currentScene ) : m_currentScene( _currentScene ) {}

	//================================================================================================================================
	//================================================================================================================================	
	void EditorSelection::ConnectCallbacks( Scene& _clientScene, Scene& _serverScene )
	{
		Input::Get().Manager().FindEvent( "delete" )->Connect( &EditorSelection::DeleteSelection, this );
 		_clientScene.onDeleteSceneNode.Connect( &EditorSelection::OnSceneNodeDeleted, this );
 		_serverScene.onDeleteSceneNode.Connect( &EditorSelection::OnSceneNodeDeleted, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::DeleteSelection()
	{
		if ( m_selectedSceneNode != nullptr &&  ! m_selectedSceneNode->IsRoot() )
		{
			m_selectedSceneNode->scene->DeleteSceneNode( *m_selectedSceneNode );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Update( const bool _gameWindowHovered )
	{
		SCOPED_PROFILE( selection );

		//RunSystem<SceneNode>( 42.f, std::string(" bwa") );

		bool mouseCaptured = false;
		

		// translation gizmo on selected scene node
		if ( m_selectedSceneNode != nullptr && m_selectedSceneNode != &m_currentScene->GetMainCamera() )
// 			 && m_selectedSceneNode->GetComponent<UIMeshRenderer>() == nullptr @hack
// 			 && m_selectedSceneNode->GetComponent<UITransform>() == nullptr )
		{
			EcsWorld& world = m_selectedSceneNode->scene->GetWorld();
			EntityID entityID = world.GetEntityID( m_selectedSceneNode->entityHandle );
			if( world.HasComponent<Transform2>( entityID ) )
			{
				Transform2& transform = world.GetComponent< Transform2 >( entityID );
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


			EcsWorld& world = m_currentScene->GetWorld();
			EntityID cameraID = world.GetEntityID( m_currentScene->GetMainCamera().entityHandle );
			const Transform2& cameraTransform = world.GetComponent<Transform2>( cameraID );
			const Camera2& camera = world.GetComponent<Camera2>( cameraID );

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