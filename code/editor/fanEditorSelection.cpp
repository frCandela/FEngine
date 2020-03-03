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
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanScene.hpp"
#include "render/fanMesh.hpp"

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
// 		_clientScene.onDeleteGameobject.Connect( &EditorSelection::OnSceneNodeDeleted, this ); @node
// 		_serverScene.onDeleteGameobject.Connect( &EditorSelection::OnSceneNodeDeleted, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::DeleteSelection()
	{
		if ( m_selectedSceneNode != nullptr )
		{
			//m_currentScene->DeleteGameobject( m_selectedSceneNode ); @node
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Update( const bool _gameWindowHovered )
	{
		SCOPED_PROFILE( selection )

		bool mouseCaptured = false;

		// Translation gizmo on selected gameobject
// 		if ( m_selectedSceneNode != nullptr && m_selectedSceneNode != &m_currentScene->GetMainCamera().GetGameobject()
// 			 && m_selectedSceneNode->GetComponent<UIMeshRenderer>() == nullptr
// 			 && m_selectedSceneNode->GetComponent<UITransform>() == nullptr )
// 
// 		{
// 			Transform* transform = m_selectedSceneNode->GetComponent< Transform >();
// 			btVector3 newPosition;
// 			if ( EditorGizmos::Get().DrawMoveGizmo( btTransform( btQuaternion( 0, 0, 0 ), transform->GetPosition() ), ( size_t ) this, newPosition ) )
// 			{
// 				transform->SetPosition( newPosition );
// 				mouseCaptured = true;
// 			}
// 		} @node

		// Mouse selection
		if ( !mouseCaptured && _gameWindowHovered && Mouse::Get().GetButtonPressed( Mouse::button0 ) )
		{
			const btVector3 cameraOrigin = m_currentScene->GetMainCamera().GetGameobject().GetTransform().GetPosition();
			const Ray ray = m_currentScene->GetMainCamera().ScreenPosToRay( Mouse::Get().GetScreenSpacePosition() );
			const std::vector<Gameobject*>& entities = m_currentScene->BuildEntitiesList();

			// raycast on all the entities
			Gameobject* closestGameobject = nullptr;
			float closestDistance2 = std::numeric_limits<float>::max();
			for ( int gameobjectIndex = 0; gameobjectIndex < entities.size(); gameobjectIndex++ )
			{
				Gameobject* gameobject = entities[ gameobjectIndex ];

				if ( gameobject == &m_currentScene->GetMainCamera().GetGameobject() )
				{
					continue;
				}

				const AABB& aabb = gameobject->GetAABB();
				btVector3 intersection;
				if ( aabb.RayCast( ray.origin, ray.direction, intersection ) == true )
				{
					MeshRenderer* meshRenderer = gameobject->GetComponent<MeshRenderer>();
					if ( meshRenderer != nullptr && meshRenderer->GetMesh() != nullptr )
					{
						Transform& transform = gameobject->GetTransform();
						const Ray transformedRay( transform.InverseTransformPoint( ray.origin ), transform.InverseTransformDirection( ray.direction ) );
						if ( meshRenderer->GetMesh()->GetHull().RayCast( transformedRay.origin, transformedRay.direction, intersection ) == false )
						{
							continue;
						}
					}
					const float distance2 = intersection.distance2( cameraOrigin );
					if ( distance2 < closestDistance2 )
					{
						closestDistance2 = distance2;
						closestGameobject = gameobject;
					}
				}
			}
			//SetSelectedSceneNode( closestGameobject ); @node
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